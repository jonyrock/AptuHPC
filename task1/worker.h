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

    Worker(WorkerTraits* traits, boost::function< void (WorkerTraits*) > dieHandler) :
    m_traits(traits), m_dieHandler(dieHandler), m_taskAvailable(false), m_isFree(true),
    m_thread(boost::bind(&Worker::workerRun, this)) {
    }

    void workerRun() {

        float waited = 0.0f;

        while (true) {
            if(m_traits->isKilled())
                return;

            m_taskMutex.lock();
            auto fun = m_task;
            bool taskAvailable = m_taskAvailable;
            if (taskAvailable) {
                m_taskAvailable = false;
                m_isFree = false;
            }
            m_taskMutex.unlock();

            if (taskAvailable) {
                fun();
                m_taskMutex.lock();
                m_isFree = true;
                m_taskMutex.unlock();
                waited = 0.0f;
            } else {
                sleep(0.01f);
                std::cout << "goto sleep " << std::endl;
                waited += 0.01f;
                if (waited >= 60.0f)
                    break;
            }

        }



    }

    WorkerTraits* traits() {
        return m_traits;
    }

    bool setTask(boost::function< void () > f) {
        boost::lock_guard<boost::mutex> gurad(m_taskMutex);
        if (!m_isFree)
            return false;
        if (!m_taskAvailable) {
            m_task = f;
            m_taskAvailable = true;
            return true;
        }
        return false;
    }

    void kill() {
        if(m_traits->isKilled())
            return;
        m_thread.interrupt();
        m_traits->isKilled(true);
    }



private:
    WorkerTraits* m_traits;
    boost::function< void (WorkerTraits*) > m_dieHandler;
    boost::thread m_thread;

    boost::mutex m_taskMutex;
    bool m_taskAvailable;
    boost::function< void () > m_task;
    bool m_isFree;

};