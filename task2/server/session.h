#pragma once 

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <string>

using boost::asio::ip::tcp;

class session
  : public boost::enable_shared_from_this<session>,
    private boost::noncopyable {

public:
	session(boost::asio::io_service& service):
    m_socket(service),
    m_isGood(true)
  {}

  inline boost::asio::ip::tcp::socket& socket() {
    return m_socket;
  }

  void start();
  
  void sendMessage(const std::string& message);
  
  inline void messageEvent(
    boost::function<void (const std::string message)> fun) {
    m_messageEvent = fun;
  }
  
  inline void deathEvent(boost::function<void ()> fun) {
    m_deathEvent = fun;
  }

  ~session();

private:
  
  void addRead();
  
  void handleReadHandshake(
    const boost::system::error_code& error,
    size_t bytes_transferred
  );
  
  void deleteThis();

  void handleWriteHandshake(
    const boost::system::error_code& error
  );

  void handleReadMessage(
    const boost::system::error_code& error,
    size_t bytes_transferred
  );
  
  void handleWriteMessage(
    const boost::system::error_code& error
  );

  boost::asio::ip::tcp::socket m_socket;
  enum { MAXLENGTH = 1024 };
  char m_data[MAXLENGTH];
  char m_dataOut[MAXLENGTH];
  bool m_isGood;
  boost::function<void (const std::string message)> m_messageEvent;
  boost::function<void ()> m_deathEvent;
  
  boost::mutex m_sendMutex;
  std::atomic_bool m_isOpen;

};

typedef boost::shared_ptr<session> session_ptr;