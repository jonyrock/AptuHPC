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
                    if(!m_taskCondition.timed_wait(lock, m_timeout))
                        break;
                }

            }

        }
        triggerKill();
    }

    WorkerTraits* traits() {
        return m_traits;
    }

    bool setTask(boost::function< void () > f) {
        if (!m_taskMutex.try_lock())
            return false;
        m_task = f;
        m_taskAvailable = true;
        m_taskCondition.notify_one();
        m_taskMutex.unlock();
        return true;
    }

    void kill() {
        boost::lock_guard<boost::mutex> guard(m_taskMutex);
        triggerKill();
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
    boost::function< void () > m_task;

    void triggerKill() {
        if (m_traits->isKilled())
            return;
        m_thread.interrupt();
        m_traits->isKilled(true);
    }

};