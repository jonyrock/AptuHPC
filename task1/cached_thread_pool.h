#pragma once

#include "worker_traits.h"
#include "worker.h"

#include <atomic>
#include <vector>

using namespace std;

class CachedThreadPool {
public:

    CachedThreadPool(size_t hotWorkersCount,
            boost::posix_time::time_duration timeout) :
    m_timeout(timeout) {
        m_workersActive.store(0);
        m_workersKilled.store(0);
        m_workersIdNum.store(0);
        createHotWorkers(hotWorkersCount);
        m_collectorThread = boost::thread(boost::bind(&CachedThreadPool::collectorRunner, this));
    }

    size_t workersCount() const {
        return m_workersActive.load();
    }

    size_t addTask(size_t taskId, boost::function< void () > f) {
        for (auto wp : m_workers) {
            if (wp->isDead()) continue;
            if (wp->worker->setTask(taskId, f)) {
                return wp->id;
            }
        }
        return addNewWorker(taskId, f);
    }

    void killTask(size_t id) {
        boost::lock_guard<boost::mutex> gurad(m_workersMutex);
        for (auto wp : m_workers) {
            if (wp->worker->currentTaskId() == id) {
                wp->worker->killTask(id);
            }
        }
    }

    void killAllTasks() {
        boost::lock_guard<boost::mutex> gurad(m_workersMutex);
        for (auto wp : m_workers) {
            delete wp->worker;
        }
    }

    ~CachedThreadPool() {
        m_collectorThread.interrupt();
        killAllTasks();
    }

private:
    vector<WorkerTraits*> m_workers;
    boost::mutex m_workersMutex;
    atomic_size_t m_workersActive;
    atomic_size_t m_workersKilled;
    atomic_size_t m_workersIdNum;
    boost::thread m_collectorThread;
    boost::posix_time::time_duration m_timeout;

    void createHotWorkers(size_t count) {
        boost::lock_guard<boost::mutex> guard(m_workersMutex);
        for (size_t i = 0; i < count; i++) {
            auto traits = new WorkerTraits();
            traits->id = m_workersIdNum.fetch_add(1);
            auto worker = new Worker(
                    traits,
                    true, boost::posix_time::millisec(0));
            traits->worker = worker;
            m_workers.push_back(traits);
        }
    }

    size_t addNewWorker(size_t taskId, boost::function< void () > f) {
        auto traits = new WorkerTraits();
        traits->id = m_workersIdNum.fetch_add(1);
        auto worker = new Worker(traits,
                false, m_timeout);
        traits->worker = worker;
        worker->setTask(taskId, f); // i own it
        boost::lock_guard<boost::mutex> gurad(m_workersMutex);
        m_workers.push_back(traits);
        return traits->id;
    }

    void triggerCollector() {
        cout << "collector trigger" << endl;
        boost::lock_guard<boost::mutex> gurad(m_workersMutex);
        vector<WorkerTraits*> v;
        for (size_t i = 0; i < m_workers.size(); i++) {
            if (m_workers[i]->isDead()) {
                m_workersKilled.fetch_sub(1);
                m_workersActive.fetch_sub(1);
                delete m_workers[i]->worker;
                delete m_workers[i];
            } else {
                v.push_back(m_workers[i]);
            }
        }
        m_workers = v;
    }

    void collectorRunner() {
        while (true) {
            try {
                boost::this_thread::sleep(boost::posix_time::seconds(20));
            } catch (const boost::thread_interrupted e) {
                triggerCollector();
                return;
            }
            triggerCollector();
        }
    }

};
