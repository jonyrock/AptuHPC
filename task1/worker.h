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
            bool isHot, boost::posix_time::time_duration timeout) :
    m_traits(traits), m_isHot(isHot), m_timeout(timeout), 
            m_taskAvailable(false),
    m_taskCondition(), m_thread(boost::bind(&Worker::workerRun, this)) {
    }

    void workerRun() {
        boost::unique_lock<boost::mutex> lock(m_taskMutex);
        while (true) {
            try {
                if (m_taskAvailable) {
                    m_taskAvailable = false;
                    m_task();
                    m_taskId = 0;
                } else {
                    if (m_isHot) {
                        m_taskCondition.wait(lock);
                    } else {
                        if (!m_taskCondition.timed_wait(lock, m_timeout))
                            break;
                    }
                }
            } catch (const boost::thread_interrupted& e) {
                cout << "got interrupted " << m_taskId << endl;
                continue;
            }
        }
        cout << "Im outtie " << m_traits->id << endl;
        m_traits->isDead(true);
    }

    WorkerTraits* traits() {
        return m_traits;
    }

    bool setTask(size_t id, boost::function< void () > f) {
        //        cout << "gonna set" << endl;
        if (m_taskMutex.try_lock() == false)
            return false;
        m_task = f;
        m_taskAvailable = true;
        m_taskCondition.notify_one();
        m_taskMutex.unlock();
        m_taskId = id;
        return true;
    }

    void killTask(size_t taskId) {
        if (m_taskId != taskId)
            return;
        m_thread.interrupt();
    }

    size_t currentTaskId() {
        return m_taskId;
    }
    
    ~Worker() {
        m_thread.join();
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

};