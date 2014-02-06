
#include "ClientApp.h"

#include <iostream>
#include <csignal>

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
    
    app = new ClientApp();
    signal(SIGINT, signalHandler);
    app->run();
    delete app;
    
}