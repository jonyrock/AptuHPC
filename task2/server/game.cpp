#include "game.h"
#include "server.h"
#include <string>

using namespace std;

game::game(short port, size_t workersNumber):m_server(this, port) {
	m_server.run(workersNumber);
}

void game::onClientMessage(size_t clientId, const string& params) {
	
}

void game::onClientDead(size_t clientId) {
	
}