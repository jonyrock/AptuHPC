#include "game.h"
#include "server.h"
#include <string>
#include <sstream>

using namespace std;

game::game(short port, size_t workersNumber):m_server(this, port) {
	m_server.run(workersNumber);
}

void game::onClientMessage(size_t clientId, const string& message) {
	cout << "client [" << clientId << "] says:" << message << endl;
	stringstream ss;
	ss << ":hello " << clientId;
	m_server.messageToClient(clientId, ss.str());
}

void game::onClientDead(size_t clientId) {
	cout << "client [" << clientId << "] is offline" << endl;
}