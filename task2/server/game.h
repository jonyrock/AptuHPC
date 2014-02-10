#pragma once

#include "igame.h"
#include "server.h"
#include <string>
#include <map>

class game : public igame {
	
	struct heroState {
		size_t x;
        size_t y;
        char color[7];
	};
	
public:
	game(short port, size_t workersNumber);
	
	void onClientMessage(size_t clientId, const std::string& message);
	
	void onClientDead(size_t clientId);
	
private:
    std::map<size_t, heroState> players;
	server m_server;
    
};