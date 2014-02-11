#include "game.h"
#include "server.h"
#include <string>
#include <sstream>
// #include <boost/algorithm/string/predicate.hpp>

using namespace std;

game::game(short port, size_t workersNumber):m_server(this, port) {
	m_server.run(workersNumber);
}

string makeAppear(size_t clientId, const game::heroState& state) {
	stringstream ss;
	ss << ":appear " << clientId << " "
	   << state.color << " " 
       << state.x << " " << state.y;
    return ss.str();
}

void game::onClientMessage(size_t clientId, const string& message) {
	cout << "client [" << clientId << "] says: " << message << endl;
	
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
		
		m_server.messageBroadcastExcept(
			clientId, 
			makeAppear(clientId, state)
		);
		
		for(auto pr: m_players) {
			m_server.messageToClient(
				clientId,
				makeAppear(pr.first, pr.second)
			);
		}
			
		m_players.insert(make_pair(clientId, state));
		
		return;
	}
	
	if(command == ":update") {
		if(m_players.find(clientId) == m_players.end()) {
			return;
		}
		heroState state = m_players[clientId];
		ssIn >> state.x >> state.y;
		ssOutBroadcast << ":update " << clientId << " "
					   << state.x << " " << state.y;
		m_players[clientId] = state;
		m_server.messageBroadcastExcept(clientId, ssOutBroadcast.str());
		return;
	}

}

void game::onClientDead(size_t clientId) {
	if(m_players.find(clientId) != m_players.end())
		m_players.erase(m_players.find(clientId));
	
	cout << "client [" << clientId << "] is offline" << endl;
	
	stringstream ss;
	ss << ":bye " << clientId;
	m_server.messageBroadcast(ss.str());
	
}