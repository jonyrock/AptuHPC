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
        ConsoleInterface(boost::bind(&CachedThreadsApp::ciAddHandler, this, _1, _2),
                boost::bind(&CachedThreadsApp::ciKillHandler, this, _1, _2),
                boost::bind(&CachedThreadsApp::ciShowHandler, this, _1));

    }

private:

    CachedThreadPool pool;

    int task_id;

    void ciAddHandler(ConsoleInterface& ci, float seconds) {
        task_id++;
        size_t myId = pool.addTask(boost::bind(CachedThreadsApp::sleep_task, seconds, task_id));
        stringstream ss;
        ss << "added task to worker " << myId << " with "
                << seconds << " seconds sleep" << endl;
        ci << ss.str();
    }

    void ciKillHandler(ConsoleInterface& ci, size_t id) {
        stringstream ss;
        ss << "kill with " << id << endl;
        ci << ss.str();
    }

    void ciShowHandler(ConsoleInterface& ci) {
        stringstream ss;
        ss << "1:(-12.0) 12:(-32.7) 14:(12.0)" << endl;
        ci << ss.str();
    }

    static void sleep_task(float seconds, int my_id) {
        cout << "enter" << endl;
        for (int i = 0; i < 5; i++) {
            cout << "I am " << my_id << endl;
            boost::this_thread::sleep(
                    boost::posix_time::millisec(seconds * 1000));
        }
    }

};

int main(int argc, char** argv) {

    CachedThreadsApp app;

    return 0;
}

