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
  socket_.async_read_some(
    boost::asio::buffer(data_, max_length),
    boost::bind(
      &session::handle_read, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred
    )
  );
}

void session::handle_read(const system::error_code& error, 
                          size_t bytes_transferred) {
  if (!error) {
    string str(data_, bytes_transferred);
    string response = buildWebSocketResponse(str);
    response.copy(data_, response.size(), 0);
    data_[response.size()] = '\0';
    cout << "Client connected" << endl;
    boost::asio::async_write (
      socket_,
      boost::asio::buffer(data_, response.size()),
      boost::bind (
        &session::handle_write, this,
        boost::asio::placeholders::error
      )
    );
  } else {
      delete this;
  }
}

void session::handle_write(const system::error_code& error) {
	if (!error) {
    cout << "something is written" << endl;
    socket_.async_read_some(
      boost::asio::buffer(data_, max_length),
      boost::bind (
      	&session::handle_read, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
      )
    );
  } else {
    cout << "write error" << endl;
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
