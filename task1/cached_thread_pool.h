#pragma once

#include "worker_traits.h"
#include "worker.h"

#include <atomic>
#include <vector>

using namespace std;

class CachedThreadPool {
public:

    CachedThreadPool() {
        m_workersActive.store(0);
        m_workersNextId.store(0);
        m_workersKilled.store(0);
    }

    size_t workersCount() const {
        return m_workersActive.load();
    }

    size_t addTask(boost::function< void () > f) {
        for (auto wp : m_workers) {
            if (wp->isKilled()) continue;
            if (wp->worker->setTask(f)) {
                return wp->id;
            }
        }
        return addNewWorker(f);
    }

private:
    vector<WorkerTraits*> m_workers;
    boost::mutex m_workersMutex;
    atomic_size_t m_workersActive;
    atomic_size_t m_workersNextId;
    atomic_size_t m_workersKilled;

    size_t addNewWorker(boost::function< void () > f) {
        auto traits = new WorkerTraits();
        traits->id = m_workersNextId.fetch_add(1);
        traits->poolPosition = m_workersActive.load();
        auto worker = new Worker(
                traits,
                boost::bind(&CachedThreadPool::onWorkerDie, this, _1)
                );
        traits->worker = worker;
        worker->setTask(f); // i own it
        boost::lock_guard<boost::mutex> gurad(m_workersMutex);
        m_workers.push_back(traits);
        return traits->id;
    }

    void triggerCollector() {
        boost::lock_guard<boost::mutex> gurad(m_workersMutex);
        vector<WorkerTraits*> v;
        for (size_t i = 0; i < m_workers.size(); i++) {
            if (m_workers[i]->isKilled()) {
                m_workersKilled.fetch_sub(1);
                m_workersActive.fetch_sub(1);
            } else {
                v.push_back(m_workers[i]);
            }
        }
        m_workers = v;
    }

    void onWorkerDie(WorkerTraits* traits) {
        delete traits->worker;
        // vector = killed | workersCount
        if (m_workersKilled.fetch_add(1) >= m_workersActive.fetch_sub(1)) {
            triggerCollector();
        }
    }

};
