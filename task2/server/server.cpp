
#include "server.h"
#include "session.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

server::server(short port):
	m_acceptor(m_service, tcp::endpoint(tcp::v4(), port)) {
}

// TODO: use real threads
void server::run(size_t workersNumber) {
	addSession();
	m_service.run();
}

void server::addSession() {
	session* newSession = new session(m_service);
	m_acceptor.async_accept(
		newSession->socket(),
		boost::bind(
			&server::handleAccept, this, newSession,
			boost::asio::placeholders::error
		)
	);
}

void server::handleAccept(session* newSession, 
  	                      const boost::system::error_code& error) {
	if (!error) {
      	newSession->start();
		addSession();
    } else {
		delete newSession;
	}
}