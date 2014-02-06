#pragma once

#include "Message.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace boost::asio;
using namespace std;

class ServerApp {
public:

    ServerApp(size_t workersNumber, size_t port) :
    m_workersNumer(workersNumber), m_sock(m_service) {
        ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), port);
        m_sock.open(ep.protocol());
        m_sock.set_option(ip::tcp::socket::reuse_address(true));
        m_sock.bind(ep);
        cout << "isOpen: " << m_sock.is_open() << endl;
    }

    void readAgain() {
        m_sock.async_receive(buffer(m_buff, 512),
                boost::bind(&ServerApp::onRead, this, _1, _2));
    }

    void run() {
        m_service.run();
    }


private:

    void onRead(const boost::system::error_code& err, size_t readBytes) {
        if (!err) {
            cout << err.message() << endl;
            return;
        }
        cout << "read " << readBytes << endl;
        readAgain();
    }

    io_service m_service;
    ip::tcp::socket m_sock;
    char m_buff[512];

    size_t m_workersNumer;
    ip::tcp::endpoint m_senderEp;

};