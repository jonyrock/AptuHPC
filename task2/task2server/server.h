#include "session.h"

#include <boost/asio.hpp>

class server {

public:
	server(short port);

	void addSession();

	void handleAccept(session* new_session,
  	                  const boost::system::error_code& error);

private:
	boost::asio::io_service m_service;
	boost::asio::ip::tcp::acceptor m_acceptor;
  
};