#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

class session {

public:
  
	session(boost::asio::io_service& service):
    m_socket(service),
    m_isGood(true) 
  {}

  inline boost::asio::ip::tcp::socket& socket() {
    return m_socket;
  }

  void start();

  void handleRead(
    const boost::system::error_code& error,
    size_t bytes_transferred
  );

  void handleWrite(const boost::system::error_code& error);
  
  ~session();

private:
	
  std::string buildWebSocketResponse(
    const std::string& request
  );

  std::string getWSKeyAccept(const std::string& key);

  boost::asio::ip::tcp::socket m_socket;
  enum { MAXLENGTH = 1024 };
  char m_data[MAXLENGTH];
  bool m_isGood;
	
};
