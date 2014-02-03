#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <boost/function.hpp>

using namespace std;

class ConsoleInterface {
public:

    ConsoleInterface(
            boost::function< void (ConsoleInterface&, float) > addHandler,
            boost::function< void (ConsoleInterface&, size_t) > killHandler,
            boost::function< void (ConsoleInterface&) > showHandler,
            boost::function< void (ConsoleInterface&) > exitHandler
            ) :
    m_addHandler(addHandler),
    m_killHandler(killHandler),
    m_showHandler(showHandler),
    m_exitHandler(exitHandler),
    m_isExit(false) {
        printWelcome();
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

private:

    void printWelcome() {
        cout << "Welcome to CachedThreadPool emulator" << endl;
        cout << "Commands available:" << endl;
        cout << "   add <seconds>" << endl;
        cout << "   kill <id>" << endl;
        cout << "   show" << endl;
        cout << "   help" << endl;
        cout << "   exit" << endl;

        cout << "--------------------------------------" << endl;
        cout << "Examples with description: " << endl;
        cout << "add 13.5 // Add new task which will sleep for 13.5 seconds" << endl;
        cout << "kill 12  // Kill task with id 12" << endl;
        cout << "show     // Show active tasks with their statuses in format:" << endl;
        cout << "         //   <workerId>[<taskId>](<seconds>) - taskId working on workerId time in seconds " << endl;
        cout << "         //   <workerId>[x](<seconds>)  - seconds waiting for new task" << endl;
        cout << "         // Output example: 7[122]:(12.0) !13[10]:(32.7) 14[x]:(12.0)" << endl;
        cout << "         // where ! tells that it is a hot one" << endl;
        cout << "help     // Show this help" << endl;
        cout << "exit     // Exit from this app" << endl;
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

            m_addHandler(*this, seconds);
            return;
        }
        if (command == "kill") {
            size_t id = 0;
            ss >> id;
            if (id == 0) {
                cout << "enter process id please. Example: kill 12" << endl;
                return;
            }

            m_killHandler(*this, id);
            return;
        }
        if (command == "show") {
            m_showHandler(*this);
            return;
        }
        if (command == "help") {
            cout << endl;
            printWelcome();
            return;
        }

        if (command == "exit") {
            m_exitHandler(*this);
            m_isExit = true;
        }

    }

    void startInterfaceLoop() {
        while (true) {
            string line;
            cout << endl;
            cout << "> ";
            getline(cin, line);
            consumeLine(line);
            if (m_isExit)
                break;
        }
    }

    boost::function< void (ConsoleInterface&, float) > m_addHandler;
    boost::function< void (ConsoleInterface&, size_t) > m_killHandler;
    boost::function< void (ConsoleInterface&) > m_showHandler;
    boost::function< void (ConsoleInterface&) > m_exitHandler;

    bool m_isExit;

};