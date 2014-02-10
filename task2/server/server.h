#pragma once 

#include "session.h"
#include "igame.h"

#include <boost/asio.hpp>

#include <vector>

class server {

public:
	server(igame* game, short port);

	void run(size_t workersNumber);

	void addSession();

	void handleAccept(session* new_session,
  	                  const boost::system::error_code& error);

private:
	boost::asio::io_service m_service;
	igame* m_game;
	boost::asio::ip::tcp::acceptor m_acceptor;
	
	std::vector<session> m_sessions;
  
};