#pragma once

#include "igame.h"
#include "server.h"
#include <string>

class game : public igame {
public:
	game(short port);
	
	void onClientHello(size_t clientId, const std::string& params);
	void onClientUpdate(size_t clientId, const std::string& params);
	void onClientDead(size_t clientId);
	
private:
	server m_server;
};