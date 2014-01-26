#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <boost/function.hpp>

using namespace std;

class ConsoleInterface {
public:

    ConsoleInterface(
            boost::function< void (float) > addHandler,
            boost::function< void (size_t) > killHandler
            ) :
    m_addHandler(addHandler),
    m_killHandler(killHandler) {
        printWelcome();
        startInterfaceLoop();
    }

private:

    void printWelcome() {
        cout << "Welcome to CachedThreadPool emulator" << endl;
        cout << "Commands available:" << endl;
        cout << "   add <seconds>" << endl;
        cout << "   kill <id>" << endl;
        cout << "   help" << endl;
        cout << "   exit" << endl;
    }

    void consumeLine(const string& line) {
        stringstream ss(line);
        string command;
        ss >> command;
        
        if (command == "add") {
            float seconds = 0;
            
            ss >> seconds;
            
            if (seconds == 0) {
                cout << "enter sleep duration in seconds please. Example: add 12.3" << endl;
                return;
            }
            
            m_addHandler(seconds);
            return;
        }
        if (command == "kill") {
            size_t id = 0;
            ss >> id;
            if (id == 0) {
                cout << "enter process id please. Example: kill 12" << endl;
                return;
            }
            
            m_killHandler(id);
            return;
        }
        if (command == "help") {
            cout << endl;
            printWelcome();
            return;
        }

        if (command == "exit") {
            exit(0);
        }
        
        

    }

    void startInterfaceLoop() {

        while (true) {
            string line;
            cout << endl;
            cout << "> ";
            getline(cin, line);
            consumeLine(line);
        }
    }

    boost::function<void (float) > m_addHandler;
    boost::function<void (size_t) > m_killHandler;

};