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
	
	
	void broadcastMessage(const std::string& message);

private:
	boost::asio::io_service m_service;
	igame* m_game;
	boost::asio::ip::tcp::acceptor m_acceptor;
	
	// TODO: concurent map
	std::vector<session*> m_sessions;
	
	// TODO: atomic
	size_t m_sessionsCouner;
	
	inline void onClientMessage(
		size_t clientId, const std::string& message
	) {	
		m_game->onClientMessage(clientId, message);
	}
	
	void onClientDead(size_t clientId) {
		m_game->onClientDead(clientId);
	}
	
};