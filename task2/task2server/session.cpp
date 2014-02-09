#include "session.h"
#include "handshake.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <iostream>


using namespace std;
using namespace boost;
using namespace boost::asio;
using boost::asio::ip::tcp;


void session::start() {
  cout << "start" << endl;
  m_socket.async_read_some(
    boost::asio::buffer(m_data, MAXLENGTH),
    boost::bind(
      &session::handleReadHandshake, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred
    )
  );
}

void session::handleReadHandshake(
  const system::error_code& error, 
  size_t bytes_transferred) {

  if (!error) {
    string str(m_data, bytes_transferred);
    
    // cout << "read " << "----------------" << endl;
    // cout << str << endl;
    // cout << "xxxxxxxxx" << endl;
    
    if(!boost::starts_with(str, "GET")) {
      m_isGood = false;
      delete this;
    }
    
    string response = handshake::createResponse(str);
    response.copy(m_data, response.size(), 0);
    m_data[response.size()] = '\0';
    boost::asio::async_write(
      m_socket,
      boost::asio::buffer(m_data, response.size()),
      boost::bind(
        &session::handleWriteHandshake, this,
        boost::asio::placeholders::error
      )
    );
    
  } else {
      m_isGood = false;
      delete this;
  }
}

void session::handleWriteHandshake(
  const system::error_code& error) {
  if (!error) {
    m_isConnected = true;
    cout << "new connection" << endl;
    m_socket.async_read_some(
      boost::asio::buffer(m_data, MAXLENGTH),
      boost::bind (
        &session::handleReadMessage, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
      )
    );
  } else {
    m_isGood = false;
    delete this;
  }
}

void session::handleReadMessage(
  const system::error_code& error, 
  size_t bytes_transferred) {

  if (!error) {
    
    string str(m_data, bytes_transferred);
    
    cout << "got new message: ";
    cout << str << endl;
    
    m_socket.async_read_some(
      boost::asio::buffer(m_data, MAXLENGTH),
      boost::bind (
        &session::handleReadMessage, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
      )
    );
  } else {
    m_isGood = false;
    delete this;
  }
  
}


session::~session() {
  if (m_isGood)
    cout << "session is closed";
  else
    cout << "session is broken";
}