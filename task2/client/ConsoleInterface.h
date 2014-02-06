#pragma once

#include "Message.h"

#include <iostream>
#include <string>
#include <sstream>
#include <boost/function.hpp>


using namespace std;

class ConsoleInterface {
public:

    ConsoleInterface(
            boost::function< void (ConsoleInterface&, const string&) > sendHandler,
            boost::function< void (ConsoleInterface&, const string&) > nameHandler
            ) :
    m_sendHandler(sendHandler),
    m_nameHandler(nameHandler),
    m_isExit(false) {
        printWelcome();
        m_userName = "ClientName";
        m_nameHandler(*this, m_userName);
        startInterfaceLoop();
        
    }

    void printMessage(const string& message) {
        cout << message;
    }

    template<typename T>
    ConsoleInterface& operator <<(const T& str) {
        cout << str;
        return *this;
    }

    void printMessage(const Message& msg) {
        stringstream ss;
        ss << "--- new message! ---" << endl;
        ss << msg.senderName << ": " << msg.message << endl;
        cout << ss.str();
    }

    ~ConsoleInterface() {
        cout << "Bye!" << endl;
    }

private:

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
        m_sendHandler(*this, line);
    }

    void startInterfaceLoop() {
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

    string m_userName;
    bool m_isExit;

    boost::function< void (ConsoleInterface&, const string&) > m_sendHandler;
    boost::function< void (ConsoleInterface&, const string&) > m_nameHandler;

};