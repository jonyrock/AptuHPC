
#include "ServerApp.h"

#include <iostream>
#include <csignal>

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
    
    app = new ServerApp();
    signal(SIGINT, signalHandler);
    app->run();
    delete app;
    
}