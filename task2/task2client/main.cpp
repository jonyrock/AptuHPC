
#include "ClientApp.h"

#include <iostream>
#include <csignal>
#include <boost/asio.hpp>

using namespace std;

ClientApp* app;

void signalHandler(int signum) {
    if (app != NULL) {
        cout << endl;
        delete app;
    }
    exit(0);
}

int main() {
    try {
        app = new ClientApp();
    } catch (const boost::system::system_error& e) {
        cout << "Connection rejected. Maybe the server is switched off?";
        cout << endl;
        exit(1);
    }
    signal(SIGINT, signalHandler);
    app->run();
    delete app;
}










