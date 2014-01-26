#include "console_interface.h"
#include "cached_thread_pool.h"

#include <iostream>
#include <sstream>

using namespace std;

void addHandler(ConsoleInterface& ci, float seconds) {
    stringstream ss;
    ss << "add with " << seconds << endl;
    ci << ss.str();
}

void killHandler(ConsoleInterface& ci, size_t id) {
    stringstream ss;
    ss << "kill with " << id << endl;
    ci << ss.str();
}

void showHandler(ConsoleInterface& ci) {
    stringstream ss;
    ss << "1:(-12.0) 12:(-32.7) 14:(12.0)" << endl;
    ci << ss.str();
}

int main(int argc, char** argv) {

    ConsoleInterface ci(addHandler, killHandler, showHandler);

    return 0;
}

