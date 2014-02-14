#include "session.h"
#include "websocketTools.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#include <iostream>
#include <string>
#include <sstream>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::asio;
using boost::asio::ip::tcp;

typedef boost::lock_guard<boost::mutex> lg;

void session::start() {
  cout << "start" << endl;
  
  m_socket.async_read_some(
    boost::asio::buffer(m_data, MAXLENGTH),
    boost::bind(
      &session::handleReadHandshake, shared_from_this(),
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
        
    if(!boost::starts_with(str, "GET")) {
      m_isGood = false;
      deleteThis();
      return;
    }
    
    string response = websocketTools::createHandshakeResponseHeader(str);
    response.copy(m_data, response.size(), 0);
    m_data[response.size()] = '\0';
    boost::asio::async_write(
      m_socket,
      boost::asio::buffer(m_data, response.size()),
      boost::bind(
        &session::handleWriteHandshake, shared_from_this(),
        boost::asio::placeholders::error
      )
    );

  } else {
      m_isGood = false;
      deleteThis();
  }
}

void session::addRead() {
  if(!m_isOpen.load())
    return;
  m_socket.async_read_some(
    boost::asio::buffer(m_data, MAXLENGTH),
    boost::bind(
      &session::handleReadMessage, shared_from_this(),
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred
    )
  );
}

void session::handleWriteHandshake(
  const system::error_code& error) {
  if (!error) {
    m_isOpen.store(true);
    cout << "new connection" << endl;
    addRead();
  } else {
    m_isGood = false;
    deleteThis();
  }
}

void session::sendMessage(const string& message) {
  if(!m_isOpen.load())
    return;
  m_sendMutex.lock();
  size_t frameLen = websocketTools::createMassage(
    message, m_dataOut
  );
    
  boost::asio::async_write(
    m_socket,
    boost::asio::buffer(m_dataOut, frameLen),
    boost::bind(
      &session::handleWriteMessage, shared_from_this(),
      boost::asio::placeholders::error
    )
  );
}

void session::handleReadMessage(
  const system::error_code& error,
  size_t bytes_transferred) {
  
  if (!error) {
    
    string str(m_data, bytes_transferred);
    
    string message;
    websocketTools::getMessage(m_data, bytes_transferred, message);
    
    if(boost::starts_with(message, ":bye")) {
      m_isGood = true;
      deleteThis();
      return;
    }
    m_messageEvent(message);
    addRead();
    
  } else {
    m_isGood = false;
    deleteThis();
  }
}

void session::handleWriteMessage(const system::error_code& error) {
  m_sendMutex.unlock();
  if (!error) {
    // cout << "a message send!" << endl;
  } else {
    m_isGood = false;
    deleteThis();
  }
}

void session::deleteThis() {
  m_isOpen.store(false);
  m_socket.close();
  if (m_isGood)
    cout << "session is closed" << endl;
  else
    cout << "session is broken" << endl;
  m_deathEvent();
}

session::~session() {
  lg lockSend(m_sendMutex);
  cout << "reall memmory free" << endl;
}