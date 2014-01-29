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
        task_id = 0;
        ConsoleInterface(boost::bind(&CachedThreadsApp::addHandler, this, _1, _2),
                boost::bind(&CachedThreadsApp::killHandler, this, _1, _2),
                boost::bind(&CachedThreadsApp::showHandler, this, _1));
        
    }

private:

    CachedThreadPool pool;

    int task_id;

    void addHandler(ConsoleInterface& ci, float seconds) {
        task_id++;
        size_t myId = pool.addTask(boost::bind(CachedThreadsApp::sleep_task, seconds, task_id));
        stringstream ss;
        ss << "added task to worker " << myId << " with " << seconds << " seconds sleep" << endl;
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

    static void sleep_task(float seconds, int my_id) {
        for (int i = 0; i < 10; i++) {
            cout << "I am" << my_id << endl;
            sleep(seconds);
        }
    }

};

int main(int argc, char** argv) {

    CachedThreadsApp app;

    return 0;
}

