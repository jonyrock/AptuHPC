#pragma once

#include "ConsoleInterface.h"
#include "Message.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>

using namespace boost::asio;

class ClientApp {
public:

    ClientApp() : m_sock(m_service) {
        
        
        
        
    }

    void run() {
        ConsoleInterface ci(
                boost::bind(&ClientApp::sendHandler, this, _1, _2),
                boost::bind(&ClientApp::nameHandler, this, _1, _2));
    }


private:

    void sendHandler(ConsoleInterface& ci, const string& msg) {
        try{
        cout << "sending " << msg << endl;
        ci.printMessage(Message(m_userName, msg));
        
        ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 80);
        m_sock.open(ip::tcp::v4());
        m_sock.connect(ep);
//        m_sock.write_some(buffer("bla ha ha bla"));
//        m_sock.close();
        } catch(const boost::system::system_error& e) {
            cout << "error occurred:" << e.what() << endl;
        }
        
    }

    void nameHandler(ConsoleInterface& ci, const string& name) {
        m_userName = name;
    }

    string m_userName;

    io_service m_service;
    ip::tcp::socket m_sock;


};