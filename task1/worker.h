#pragma once

#include "cached_thread_pool.h"
#include "worker_traits.h"

#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <atomic>
#include <iostream>

class Worker {
public:

    Worker(WorkerTraits* traits,
            boost::function< void (WorkerTraits*) > dieHandler,
            bool isHot, boost::posix_time::time_duration timeout) :
    m_traits(traits), m_isHot(isHot), m_timeout(timeout),
    m_dieHandler(dieHandler), m_taskAvailable(false),
    m_taskCondition(), m_thread(boost::bind(&Worker::workerRun, this)) {
    }

    void workerRun() {
        boost::unique_lock<boost::mutex> lock(m_taskMutex);
        while (true) {
            if (m_taskAvailable) {
                m_taskAvailable = false;
                m_task();
            } else {
                if (m_isHot) {
                    m_taskCondition.wait(lock);
                } else {
                    if (!m_taskCondition.timed_wait(lock, m_timeout))
                        break;
                }
            }
        }
        cout << "Im outtie " << m_traits->id << endl;
        m_traits->isDead(true);
        m_dieHandler(m_traits);
    }

    WorkerTraits* traits() {
        return m_traits;
    }

    bool setTask(size_t id, boost::function< void () > f) {
        cout << "gonna set" << endl;
        if (m_taskMutex.try_lock() == false)
            return false;
        m_task = f;
        m_taskAvailable = true;
        m_taskCondition.notify_one();
        m_taskMutex.unlock();
        m_taskId = id;
        return true;
    }

    void kill(size_t taskId) {

    }

    size_t currentTaskId() {
        return m_taskId;
    }


private:
    WorkerTraits* m_traits;
    boost::function< void (WorkerTraits*) > m_dieHandler;
    boost::thread m_thread;

    bool m_isHot;
    boost::posix_time::time_duration m_timeout;

    boost::mutex m_taskMutex;
    boost::condition_variable m_taskCondition;
    bool m_taskAvailable;
    bool m_taskId;
    boost::function< void () > m_task;

    void triggerKill() {

    }

};