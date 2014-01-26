#include "console_interface.h"
#include "cached_thread_pool.h"

#include <iostream>

using namespace std;

void addHandler(float seconds) {
    cout << "add with " << seconds << endl;
}

void killHandler(size_t id) {
    cout << "kill with " << id << endl;
}

int main(int argc, char** argv) {

    ConsoleInterface ci(addHandler, killHandler);

    return 0;
}

