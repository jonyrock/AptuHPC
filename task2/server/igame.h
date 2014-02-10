#pragma once

#include <string>
struct igame {
	virtual void onClientMessage(
		size_t clientId, 
		const std::string& params
	) = 0;
	virtual void onClientDead(size_t clientId) = 0;
};