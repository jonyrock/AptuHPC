#pragma once

#include "igame.h"
#include "server.h"
#include <string>

class game : public igame {
public:
	game(short port, size_t workersNumber);
	
	void onClientMessage(size_t clientId, const std::string& message);
	
	void onClientDead(size_t clientId);
	
private:
	server m_server;
};