
#include "server.h"
#include "session.h"
#include "igame.h"
#include <cassert>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <map>

using boost::asio::ip::tcp;
using namespace std;

server::server(igame* game, short port):
	m_game(game),
	m_acceptor(m_service, tcp::endpoint(tcp::v4(), port)) {
		m_sessionsCouner = 0;		
}

// TODO: use real threads
void server::run(size_t workersNumber) {
	addSession();
	m_service.run();
}

void server::messageToClient(size_t clientId, const string& message) {
	//cout << clientId << "->" << message << endl;
	assert(m_sessions.find(clientId) != m_sessions.end());
	m_sessions[clientId]->sendMessage(message);
}

void server::messageBroadcast(const string& message) {
	for(auto p: m_sessions) {
		messageToClient(p.first, message);
	}
}

void server::messageBroadcastExcept(
	size_t clientId, const string& message) {
		
	for(auto p: m_sessions) {
		if(p.first != clientId)
			messageToClient(p.first, message);
	}
	
}

void server::addSession() {
	session* newSession = new session(m_service);
	size_t newSessionId = m_sessionsCouner++;
	newSession->messageEvent(
		boost::bind(
			&server::onClientMessage, this,
			newSessionId,
			_1
		)
	);

	newSession->deathEvent(
		boost::bind(
			&server::onClientDead, this,
			newSessionId
		)
	);

	m_acceptor.async_accept(
		newSession->socket(),
		boost::bind(
			&server::handleAccept, this, 
			newSessionId, newSession,
			boost::asio::placeholders::error
		)
	);
}

void server::handleAccept(size_t newSessionId, session* newSession, 
  	                      const boost::system::error_code& error) {
	if (!error) {
		m_sessions.insert(make_pair(newSessionId, newSession));
      	newSession->start();
		addSession();
    } else {
		delete newSession;
	}
}

