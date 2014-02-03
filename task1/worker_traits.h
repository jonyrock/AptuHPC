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
#include <boost/date_time.hpp>
class Worker;

struct WorkerTraits {
    friend Worker;
    size_t id;
    size_t taskId;
    Worker* worker;
    
    boost::posix_time::ptime taskStart;
    boost::posix_time::ptime waitStart;

    WorkerTraits() {
        m_isDead.store(false);
        updateTaskStart();
        updateWaitStart();
        // like it was happen before. It is matter.
        taskStart -= boost::posix_time::seconds(1); 
    }
    
    WorkerTraits(const WorkerTraits& wt) {
        id = wt.id;
        worker = wt.worker;
        taskStart = wt.taskStart;
        waitStart = wt.waitStart;
        taskId = wt.taskId;
        m_isDead.store(wt.isDead());
    }
    
    void updateTaskStart() {
        taskStart = boost::posix_time::second_clock::local_time();
    }
    
    void updateWaitStart() {
        waitStart = boost::posix_time::second_clock::local_time();
    }
    
    bool isDead() const {
        return m_isDead.load();
    }

private:

    void isDead(bool v) {
        return m_isDead.store(v);
    }

    std::atomic_bool m_isDead;
};