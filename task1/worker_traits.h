#pragma once



/*
 * WorkerTrait is useful because 
 * 1. Pool can store some info, associated with worker without any map.
 * 2. It is possible that worker will die before it will 
 *    be added to workers list. It will be added as killed preliminary free 
 *    worker memory and be removed by collector afterwards using WorkerTraits
 *    as a little metainfo structure.
 */

#include <atomic>
class Worker;

struct WorkerTraits {
    friend Worker;
    size_t id;
    size_t poolPosition;
    Worker* worker;

    bool isKilled() {
        return m_isKilled.load();
    }

    WorkerTraits() {
        m_isKilled.store(false);
    }

private:

    void isKilled(bool v) {
        return m_isKilled.store(v);
    }

    std::atomic_bool m_isKilled;
};