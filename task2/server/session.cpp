#include "session.h"
#include "websocketTools.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#include <iostream>
#include <string>
#include <sstream>

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

void session::sendMessage(const string& message) {
  // write begin
  size_t frameLen = websocketTools::createMassage(
    message, m_dataOut
  );
    
  boost::asio::async_write(
    m_socket,
    boost::asio::buffer(m_dataOut, frameLen),
    boost::bind(
      &session::handleWriteMessage, this,
      boost::asio::placeholders::error
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
    stringstream ss(str);
    string firstWord;
    ss >> firstWord;
    
    if(firstWord != "GET") {
      m_isGood = false;
      delete this;
    }
    
    string response = websocketTools::createHandshakeResponseHeader(str);
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

void session::addRead() {
  m_socket.async_read_some(
    boost::asio::buffer(m_data, MAXLENGTH),
    boost::bind(
      &session::handleReadMessage, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred
    )
  );
}

void session::handleWriteHandshake(
  const system::error_code& error) {
  if (!error) {
    m_isConnected = true;
    cout << "new connection" << endl;
    addRead();
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
    
    string message;
    websocketTools::getMessage(m_data, bytes_transferred, message);
    
    stringstream ss(str);
    string firstWord;
    ss >> firstWord;
    
    if(firstWord == ":bye") {
      m_isGood = true;
      delete this;
      return;
    }
    m_messageEvent(message);
    addRead();
    
  } else {
    m_isGood = false;
    delete this;
  }
}

void session::handleWriteMessage(const system::error_code& error) {
  if (!error) {
    // cout << "a message send!" << endl;
  } else {
    m_isGood = false;
    delete this;
  }
}

session::~session() {
  if (m_isGood)
    cout << "session is closed" << endl;
  else
    cout << "session is broken" << endl;
  cout << "call onDeath" << endl;
  m_deathEvent();
}