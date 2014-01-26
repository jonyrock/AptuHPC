#pragma once

#include "worker.h"

#include <vector>

using namespace std;

class CachedThreadPool {
public:

    CachedThreadPool() : m_timeout(60.0f) {

    }

    size_t getWorkersCount() {
        boost::mutex::scoped_lock l(m_mutex);
        return m_workers.size();
    }

private:
    boost::mutex m_mutex;
    vector<Worker> m_workers;
    volatile float m_timeout;

};
