#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

class session {

public:
  
	session(boost::asio::io_service& service)
		:socket_(service) {}

  inline boost::asio::ip::tcp::socket& socket() {
    return socket_;
  }

  void start();

  void handle_read(
    const boost::system::error_code& error,
    size_t bytes_transferred
  );

  void handle_write(const boost::system::error_code& error);

private:
	
  std::string buildWebSocketResponse(
    const std::string& request
  );

  std::string getWSKeyAccept(const std::string& key);

  boost::asio::ip::tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
	
};
