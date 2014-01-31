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
    }

    size_t workersCount() const {
        return m_workersActive.load();
    }

    size_t addTask(size_t id, boost::function< void () > f) {
        for (auto wp : m_workers) {
            if (wp->isDead()) continue;
            if (wp->worker->setTask(id, f)) {
                return wp->id;
            }
        }
        return addNewWorker(id, f);
    }

private:
    vector<WorkerTraits*> m_workers;
    boost::mutex m_workersMutex;
    atomic_size_t m_workersActive;
    atomic_size_t m_workersKilled;
    atomic_size_t m_workersIdNum;
    boost::posix_time::time_duration m_timeout;

    void createHotWorkers(size_t count) {
        boost::lock_guard<boost::mutex> guard(m_workersMutex);
        for (size_t i = 0; i < count; i++) {
            auto traits = new WorkerTraits();
            traits->id = m_workersIdNum.fetch_add(1);
            auto worker = new Worker(
                    traits,
                    boost::bind(&CachedThreadPool::onWorkerDie, this, _1),
                    true, boost::posix_time::millisec(0));
            traits->worker = worker;
            m_workers.push_back(traits);
        }
    }

    size_t addNewWorker(size_t id, boost::function< void () > f) {
        auto traits = new WorkerTraits();
        traits->id = m_workersIdNum.fetch_add(1);
        auto worker = new Worker(
                traits,
                boost::bind(&CachedThreadPool::onWorkerDie, this, _1),
                false, m_timeout);
        traits->worker = worker;
        worker->setTask(id, f); // i own it
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
                delete m_workers[i];
            } else {
                v.push_back(m_workers[i]);
            }
        }
        m_workers = v;
    }

    void onWorkerDie(WorkerTraits* traits) {
        delete traits->worker;
        // vector = workersKilled + workersActive
        if (m_workersKilled.fetch_add(1) >= m_workersActive.fetch_sub(1)) {
//            triggerCollector();
        }
    }

};
