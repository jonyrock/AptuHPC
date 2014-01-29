#pragma once

#include "worker.h"

#include <vector>

using namespace std;

class CachedThreadPool {
public:

    CachedThreadPool() {

    }

    size_t getWorkersCount() const {
        // TODO: use mutex
        return m_workers.size();
    }

private:
    // TODO: use a mutex for Workers
    // boost::mutex m_mutex;
    vector<Worker> m_workers;

};
