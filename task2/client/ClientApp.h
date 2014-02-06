#pragma once

#include "ConsoleInterface.h"
#include "Message.h"

#include <boost/bind.hpp>


class ClientApp {
public:

    void run() {
        ConsoleInterface ci(
                boost::bind(&ClientApp::sendHandler, this, _1, _2),
                boost::bind(&ClientApp::nameHandler, this, _1, _2));
    }


private:

    void sendHandler(ConsoleInterface& ci, const string& msg) {
        cout << "sending " << msg << endl;
//        Message m(m_userName, msg);
        ci.printMessage(Message(m_userName, msg));
    }

    void nameHandler(ConsoleInterface& ci, const string& name) {
        m_userName = name;
    }
    
    string m_userName;
    
};