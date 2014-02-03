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
        cout << "worker created " << m_traits->id << endl;
        while (true) {
            try {
                if (m_taskAvailable) {
                    m_traits->updateTaskStart();
                    m_taskAvailable = false;
                    cout << "task started " << m_taskId << endl;
                    m_task();
                    cout << "task finished " << m_taskId << endl;
                    m_taskId = 0;
                    m_traits->updateWaitStart();
                } else {
                    if (m_isHot) {
                        m_taskCondition.wait(lock);
                    } else {
                        if (!m_taskCondition.timed_wait(lock, m_timeout))
                            break;
                    }
                }
            } catch (const boost::thread_interrupted& e) {
                if (m_traits->isDead())
                    break;
                cout << "task interrupted " << m_taskId << endl;
            }
        }
        cout << "worker is going to die " << m_traits->id << endl;
        m_traits->isDead(true);
    }

    WorkerTraits* traits() {
        return m_traits;
    }

    bool setTask(size_t taskId, boost::function< void () > f) {
        if (m_taskMutex.try_lock() == false)
            return false;
        cout << "set task " << m_taskId << endl;
        m_traits->taskId = taskId;
        m_taskId = taskId;
        m_task = f;
        m_taskAvailable = true;
        m_taskMutex.unlock();
        m_taskCondition.notify_one();
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
        m_traits->isDead(true);
        m_thread.interrupt();
        m_thread.join();
        cout << "worker deleted " << m_traits->id << endl;
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
    size_t m_taskId;
    boost::function< void () > m_task;

};