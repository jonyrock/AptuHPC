#pragma once 

#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

class session {

public:
  
	session(boost::asio::io_service& service):
    m_socket(service),
    m_isGood(true),
    m_isConnected(false)
  {}

  inline boost::asio::ip::tcp::socket& socket() {
    return m_socket;
  }

  void start();

  void handleReadHandshake(
    const boost::system::error_code& error,
    size_t bytes_transferred
  );

  void handleWriteHandshake(
    const boost::system::error_code& error
  );

  void handleReadMessage(
    const boost::system::error_code& error,
    size_t bytes_transferred
  );
  
  // TODO: implement it
  void handleWriteMessage(
    const boost::system::error_code& error
  );

  ~session();

private:

  boost::asio::ip::tcp::socket m_socket;
  enum { MAXLENGTH = 1024 };
  char m_data[MAXLENGTH];
  bool m_isGood;
  bool m_isConnected;

};
