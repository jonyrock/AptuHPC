#include "server.h"
#include "session.h"
#include "igame.h"
#include <cassert>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <vector>
#include <signal.h>

#include <map>

using boost::asio::ip::tcp;
using namespace std;

typedef boost::lock_guard<boost::mutex> lock_guard;

server::server(igame* game, short port):
	m_game(game),
	m_acceptor(m_service, tcp::endpoint(tcp::v4(), port)) {
		m_sessionsCouner.store(0);
}

void server::run(size_t workersNumber) {
	addSession();
	vector<boost::thread*> threads;
	for(size_t i = 0; i < workersNumber; i++) {
		threads.push_back(
			new boost::thread([this]() {
				sigset_t set;
        		sigemptyset(&set);
        		sigaddset(&set, SIGINT);
        		pthread_sigmask(SIG_SETMASK, &set, NULL);
				this->m_service.run();
			})
		);
	}
	for(size_t i = 0; i < workersNumber; i++) {
		threads[i]->join();
		delete threads[i];
	}
}

void server::messageToClient(size_t clientId, const string& message) {
	lock_guard guard(m_sessionsMutex);
	unsafe_messageToClient(clientId, message);
}

void server::unsafe_messageToClient(size_t clientId, const string& message) {
	assert(m_sessions.find(clientId) != m_sessions.end());
	m_sessions[clientId]->sendMessage(message);
}

void server::messageBroadcast(const string& message){
	lock_guard guard(m_sessionsMutex);
	unsafe_messageBroadcast(message);
}

void server::unsafe_messageBroadcast(const string& message) {
	for(auto p: m_sessions) {
		unsafe_messageToClient(p.first, message);
	}
}

void server::messageBroadcastExcept(size_t clientId, 
	const string& message) {
	lock_guard guard(m_sessionsMutex);
	unsafe_messageBroadcastExcept(clientId, message);
}

void server::unsafe_messageBroadcastExcept(size_t clientId, 
	const string& message) {
	for(auto p: m_sessions) {
		if(p.first != clientId)
			unsafe_messageToClient(p.first, message);
	}
}

void server::addSession() {
	session* newSession = new session(m_service);
	size_t newSessionId = m_sessionsCouner.fetch_add(1);
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
		addClient(newSessionId, newSession);
      	newSession->start();
    } else {
		delete newSession;
	}
	addSession();
}

void server::onClientDead(size_t clientId) {
	removeClient(clientId);
	m_game->onClientDead(clientId);
}

void server::addClient(size_t clientId, session* newSession) {
	lock_guard guard(m_sessionsMutex);
	m_sessions.insert(make_pair(clientId, newSession));
}

void server::removeClient(size_t clientId) {
	lock_guard guard(m_sessionsMutex);
	if(m_sessions.find(clientId) != m_sessions.end()) {
		m_sessions.erase(m_sessions.find(clientId));
    }
}