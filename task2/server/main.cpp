
#include "ServerApp.h"

#include <iostream>
#include <csignal>
#include <boost/asio.hpp>

using namespace std;

ServerApp* app;

void signalHandler(int signum) {
    if (app != NULL) {
        cout << endl;
        delete app;
    }
    exit(0);
}

int main() {

    try {
        app = new ServerApp(10, 5555);
    } catch (const boost::system::system_error& e) {
        cout << "error occurred:" << e.what() << endl;
    }

    signal(SIGINT, signalHandler);
    app->run();
    delete app;
}