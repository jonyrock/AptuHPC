#include "game.h"
#include "server.h"
#include <string>
#include <sstream>
// #include <boost/algorithm/string/predicate.hpp>

using namespace std;

game::game(short port, size_t workersNumber):m_server(this, port) {
	m_server.run(workersNumber);
}

void game::onClientMessage(size_t clientId, const string& message) {
	cout << "client [" << clientId << "] says:" << message << endl;
	
	stringstream ssIn(message);
	stringstream ssOut;
	stringstream ssOutBroadcast;
	string command;
	
	ssIn >> command;
	
	cout << "COMMAND: " << command << endl;
	
	if(command == ":hello") {
		heroState state;
		ssIn >> state.color >> state.x >> state.y ;
		ssOut << ":hello " << clientId;
		m_server.messageToClient(clientId, ssOut.str());
		
		ssOutBroadcast << ":appear " << clientId << " "
					   << state.color << " " 
					   << state.x << " " << state.y;
		cout << "----------" << clientId << endl;
		m_server.messageBroadcastExcept(clientId, ssOutBroadcast.str());
		return;
	}
	
	// if(command == ":updateee") {
	// 	heroState& state = 
	// 	ssIn >> state.color >> state.x >> state.y ;
	// 	ssOut << ":hello " << clientId;
	// 	goto SEND_STEP;
	// }

}

void game::onClientDead(size_t clientId) {
	if(players.find(clientId) != players.end())
		players.erase(players.find(clientId));
	cout << "client [" << clientId << "] is offline" << endl;
}