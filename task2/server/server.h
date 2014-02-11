#pragma once 

#include "session.h"
#include "igame.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <atomic>

#include <map>

class server {

public:
    
    server(igame* game, short port);

    void run(size_t workersNumber);

    void messageToClient(size_t clientId, const std::string& message);
    
    void messageBroadcast(const std::string& message);
    
    void messageBroadcastExcept(
        size_t cleintId, const std::string& message);

private:
    
    void unsafe_messageToClient(size_t clientId, const std::string& message);
    
    void unsafe_messageBroadcast(const std::string& message);
    
    void unsafe_messageBroadcastExcept(size_t cleintId, 
        const std::string& message);
    
    void addSession();

    void handleAccept(size_t newSessionIdm, session* new_session,
                      const boost::system::error_code& error);
    
    boost::asio::io_service m_service;
    igame* m_game;
    boost::asio::ip::tcp::acceptor m_acceptor;

    // TODO: concurrent map
    boost::mutex m_sessionsMutex;
    std::map<size_t, session*> m_sessions;

    std::atomic_size_t m_sessionsCouner;

    inline void onClientMessage(
        size_t clientId, const std::string& message) {
        m_game->onClientMessage(clientId, message);
    }
    
    void removeClient(size_t clientId);
    
    void addClient(size_t clientId, session* newSession);
    
    void onClientDead(size_t clientId);
    
};