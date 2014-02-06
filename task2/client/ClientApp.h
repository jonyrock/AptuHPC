#pragma once

#include "Message.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <string>

using namespace boost::asio;
using namespace std;

class ClientApp {
public:

    ClientApp(string userName = "ClientName") : m_userName(userName),
    m_sock(m_service) {
        m_isExit = false;
        ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 5555);
        m_sock.connect(ep);
    }

    void run() {
        printWelcome();
        interfaceLoop();
    }

    ~ClientApp() {
        if (m_sock.is_open())
            m_sock.close();
    }

private:

    string m_userName;
    bool m_isExit;
    io_service m_service;
    ip::tcp::socket m_sock;

    void sendHandler(const string& msg) {
        try {
            m_sock.write_some(buffer(msg));
        } catch (const boost::system::system_error& e) {
            cout << "error occurred:" << e.what() << endl;
        }
    }

    void printWelcome() {
        cout << "Welcome to MyChat client" << endl;
        cout << "Just enter some text to send a message " << endl;
        cout << "Commands available:" << endl;
        cout << "   :exit" << endl;

        cout << "--------------------------------------" << endl;
        cout << "Examples with description: " << endl;
        cout << ":exit     // Exit from this app" << endl;
    }

    void consumeLine(const string& line) {
        stringstream ss(line);
        string command;
        ss >> command;
        if (command == ":exit") {
            m_isExit = true;
            return;
        }
        sendHandler(line);
    }

    void interfaceLoop() {
        while (true) {
            cout << endl;
            cout << m_userName + "> ";
            string line;
            getline(cin, line);
            consumeLine(line);
            if (m_isExit)
                break;
        }
    }

};