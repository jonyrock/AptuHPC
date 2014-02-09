#include "session.h"
#include "base64.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <openssl/sha.h>

#include <iostream>


using namespace std;
using namespace boost;
using boost::asio::ip::tcp;


void session::start() {
  cout << "start" << endl;
  m_socket.async_read_some(
    boost::asio::buffer(m_data, MAXLENGTH),
    boost::bind(
      &session::handleRead, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred
    )
  );
}

void session::handleRead(const system::error_code& error, 
                          size_t bytes_transferred) {
  
  if (!error) {
    string str(m_data, bytes_transferred);
    
    cout << "read " << "----------------" << endl;
    cout << str << endl;
    cout << "xxxxxxxxx" << endl;
    
    string response = buildWebSocketResponse(str);
    response.copy(m_data, response.size(), 0);
    m_data[response.size()] = '\0';
    cout << "Client connected" << endl;
    boost::asio::async_write (
      m_socket,
      boost::asio::buffer(m_data, response.size()),
      boost::bind (
        &session::handleWrite, this,
        boost::asio::placeholders::error
      )
    );
  } else {
      m_isGood = false;
      delete this;
  }
}

void session::handleWrite(const system::error_code& error) {
	if (!error) {
    cout << "something is written" << endl;
    m_socket.async_read_some(
      boost::asio::buffer(m_data, MAXLENGTH),
      boost::bind (
      	&session::handleRead, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
      )
    );
  } else {
    m_isGood = false;
    delete this;
  }
}

string session::buildWebSocketResponse(const string& request) {
  stringstream requestStream(request);
  string key;
  while(requestStream.good()) {
    string line;
    getline(requestStream, line);
    if (boost::starts_with(line, "Sec-WebSocket-Key")) {
      stringstream ss(line);
      ss >> key; // skip the first
      ss >> key;
    }
  }

  stringstream responseStream;
  responseStream << "HTTP/1.1 101 Switching Protocols\r\n";
  responseStream << "Upgrade: websocket\r\n";
  responseStream << "Connection: Upgrade\r\n";
  responseStream << "Sec-WebSocket-Accept: " 
                 << getWSKeyAccept(key) << "\r\n";
  responseStream << "\r\n";

  return responseStream.str();
}

string session::getWSKeyAccept(const string& key) {
  string str(key);
	str += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1((unsigned char *)str.c_str(), str.size(), hash);
	return base64_encode(hash, SHA_DIGEST_LENGTH);
}


session::~session() {
  if (m_isGood)
    cout << "session is closed";
  else
    cout << "session is broken";
}