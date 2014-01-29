#include "console_interface.h"
#include "cached_thread_pool.h"

#include <iostream>
#include <sstream>

#include <boost/thread.hpp> // only for sleep function
#include <boost/bind.hpp>

using namespace std;

class CachedThreadsApp {
public:

    CachedThreadsApp() {
        ConsoleInterface(boost::bind(&CachedThreadsApp::addHandler, this, _1, _2),
                boost::bind(&CachedThreadsApp::killHandler, this, _1, _2),
                boost::bind(&CachedThreadsApp::showHandler, this, _1));
        
    }

private:

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

    static void sleep_task(int my_id) {
        for (int i = 0; i < 10; i++) {

            cout << "I am" << my_id << endl;
            //            boost::this_thread::sleep(1);
        }
    }

};

int main(int argc, char** argv) {

    CachedThreadsApp app;

    return 0;
}

