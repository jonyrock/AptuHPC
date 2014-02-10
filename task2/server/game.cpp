#include "game.h"
#include "server.h"
#include <string>

using namespace std;

game::game(short port):m_server(port) {
	m_server.run(10);
}

void game::onClientHello(size_t clientId, const string& params) {
	
}

void game::onClientUpdate(size_t clientId, const string& params) {
	
}

void game::onClientDead(size_t clientId) {

}