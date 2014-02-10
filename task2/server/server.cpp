
#include "server.h"
#include "session.h"
#include "igame.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>

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
			&server::handleAccept, this, newSession,
			boost::asio::placeholders::error
		)
	);
}

void server::handleAccept(session* newSession, 
  	                      const boost::system::error_code& error
) {
	if (!error) {
      	newSession->start();
		addSession();
    } else {
		delete newSession;
	}
}

void server::broadcastMessage(const string& message) {
	cout << "TODO: implement broadcast -> " << message << endl;
}