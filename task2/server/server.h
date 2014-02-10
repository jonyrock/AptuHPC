#pragma once 

#include "session.h"

#include <boost/asio.hpp>

#include <vector>

class server {

public:
	server(short port);

	void run(size_t workersNumber);

	void addSession();

	void handleAccept(session* new_session,
  	                  const boost::system::error_code& error);

private:
	boost::asio::io_service m_service;
	boost::asio::ip::tcp::acceptor m_acceptor;
	
	std::vector<session> m_sessions;
  
};