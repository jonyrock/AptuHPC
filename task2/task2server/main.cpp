
#include "session.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <cstdlib>
#include <iostream>

#include <string>
#include <sstream>


using boost::asio::ip::tcp;
using namespace std;

class server
{
public:
  server(boost::asio::io_service& io_service, short port)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
  {
    session* new_session = new session(io_service_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }

void handle_accept(session* new_session, 
  	                 const boost::system::error_code& error) {
	if (!error) {
      new_session->start();
      new_session = new session(io_service_);
      acceptor_.async_accept(new_session->socket(),
          boost::bind(&server::handle_accept, this, new_session,
            boost::asio::placeholders::error));
    } else {
		delete new_session;
    }
}

private:
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
};

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;
    // TODO: use lexical_cast
    using namespace std; // For atoi.
    server s(io_service, atoi(argv[1]));
    io_service.run();
    
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}