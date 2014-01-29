#pragma once

#include <boost/thread.hpp>
#include <boost/function.hpp>

class Worker {
public:

    Worker(size_t id) : m_id(id) {
    }

    size_t id() {
        return m_id;
    }

    bool setTask(boost::function< void () > f) {
        return false;
    }

    void kill() {

    }

    bool isFree() {
        return true;
    }

    void onDie(boost::function< void (size_t) > f) {
        m_onDie = f;
    }


private:
    const size_t m_id;
    boost::function< void (size_t) > m_onDie;

};