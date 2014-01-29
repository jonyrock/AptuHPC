#pragma once

#include "worker.h"
#include <atomic>
#include <vector>

using namespace std;

class CachedThreadPool {
public:

    CachedThreadPool() {
        workersCount.store(0);
        workersNextId.store(0);
    }

    size_t getWorkersCount() const {
        return workersCount.load();
    }

    size_t addTask(boost::function< void () > f) {
        for (auto& w : m_workers) {
            if (w.setTask(f)) {
                return w.id();
            }
        }
        return addNewWorker(f);
    }




private:
    // TODO: use a mutex for Workers
    // boost::mutex m_mutex;
    vector<Worker> m_workers;
    mutable atomic_size_t workersCount;
    mutable atomic_size_t workersNextId;

    size_t addNewWorker(boost::function< void () > f) {
        size_t myId = workersNextId.fetch_add(1);
        m_workers.push_back(Worker(myId));
        return myId;
    }

    void onWorkerDie(size_t id) {
    }

};
