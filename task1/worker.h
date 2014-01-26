#pragma once

#include <boost/thread.hpp>
#include <boost/function.hpp>

class Worker {
public:

    Worker(size_t id) : m_id(id) {

    }

    void setTask(boost::function<void () > f) {

    }

    void kill() {

    }

    bool isFree() {
        return true;
    }


private:
    const size_t m_id;

};