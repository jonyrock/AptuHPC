#pragma once

#include "igame.h"
#include "server.h"
#include <string>
#include <map>

class game : public igame {
	
public:
    
    struct heroState {
        size_t x;
        size_t y;
        char color[7];
    };
    
	game(short port, size_t workersNumber);
	
	void onClientMessage(size_t clientId, const std::string& message);
	
	void onClientDead(size_t clientId);
	
private:
    std::map<size_t, heroState> m_players;
	server m_server;
    
};