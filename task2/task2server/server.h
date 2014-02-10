#include "session.h"

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class server {

public:
	server(boost::asio::io_service& io_service, short port);

	void addSession();

	void handleAccept(session* new_session,
  	                   const boost::system::error_code& error);

private:
	boost::asio::io_service& m_service;
	tcp::acceptor m_acceptor;
  
};