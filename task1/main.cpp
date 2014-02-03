#include "console_interface.h"
#include "cached_thread_pool.h"

#include <iostream>
#include <sstream>

#include <boost/thread.hpp> 
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>

using namespace std;

class CachedThreadsApp {
public:

    CachedThreadsApp(size_t hotWorkersCount, size_t timeoutInMillseconds) :
    pool(hotWorkersCount, boost::posix_time::milliseconds(timeoutInMillseconds)) {
        m_taskIdNum = 0;
        ConsoleInterface(boost::bind(&CachedThreadsApp::ciAddHandler, this, _1, _2),
                boost::bind(&CachedThreadsApp::ciKillHandler, this, _1, _2),
                boost::bind(&CachedThreadsApp::ciShowHandler, this, _1),
                boost::bind(&CachedThreadsApp::ciExitHandler, this, _1)
                );

    }

private:

    CachedThreadPool pool;

    int m_taskIdNum;

    void ciAddHandler(ConsoleInterface& ci, float seconds) {
        m_taskIdNum++;
        size_t myId = pool.addTask(m_taskIdNum,
                boost::bind(CachedThreadsApp::sleepTaskSimple, seconds, m_taskIdNum));
        stringstream ss;
        ss << "added task " << m_taskIdNum << " to worker " << myId << " with "
                << seconds << " seconds sleep" << endl;
        ci << ss.str();
    }

    void ciKillHandler(ConsoleInterface& ci, size_t id) {
        pool.killTask(id);
        stringstream ss;
        ss << "kill with " << id << endl;
        ci << ss.str();
    }

    void ciShowHandler(ConsoleInterface& ci) {
        auto traits = pool.getTraits();
        auto curTime = boost::posix_time::second_clock::local_time();
        stringstream ss;
        for (auto trait : traits) {
            ss << trait.id;
            if (trait.taskStart > trait.waitStart) {
                auto durSeconds = (curTime - trait.taskStart).seconds();
                ss << "[" << trait.taskId << "]" << "(" << durSeconds << ")";
            } else {
                auto durSeconds = (curTime - trait.waitStart).seconds();
                ss << "[x]" << "(" << durSeconds << ")";
            }
            ss << " ";
        }
        ci << ss.str();
    }

    void ciExitHandler(ConsoleInterface& ci) {
        appExit();
    }

    void appExit() {
        cout << "terminate tasks" << endl;
        pool.killAllTasks();
        cout << "bye" << endl;
    }

    static void sleepTaskSimple(float seconds, size_t id) {
        boost::this_thread::sleep(
                boost::posix_time::millisec(seconds * 1000));
    }

    static void sleepTask(float seconds, size_t id) {
        cout << "enter " << id << endl;
        for (int i = 0; i < 5; i++) {
            cout << ":: " << id << endl;

            boost::this_thread::sleep(
                    boost::posix_time::millisec(seconds * 1000));

        }
        cout << "end " << id << endl;
    }

};

int main(int argc, char** argv) {

    if (argc != 3) {
        cout << "enter <hot threads count> and <timeout>" << endl;
    }

    size_t n = boost::lexical_cast<size_t>(argv[1]);
    size_t ts = boost::lexical_cast<size_t>(argv[2]);

    CachedThreadsApp app(n, ts);

    return 0;
}

