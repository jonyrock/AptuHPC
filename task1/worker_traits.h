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
    Worker* worker;

    WorkerTraits() {
        m_isDead.store(false);
    }

    bool isDead() {
        return m_isDead.load();
    }

private:

    void isDead(bool v) {
        return m_isDead.store(v);
    }

    std::atomic_bool m_isDead;
};