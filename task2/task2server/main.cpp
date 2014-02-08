#include <iostream>
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost;

typedef shared_ptr<ip::tcp::socket> socket_ptr;

io_service service;
ip::tcp::endpoint ep(ip::tcp::v4(), 2014);
ip::tcp::acceptor acc(service, ep);
socket_ptr sock(new ip::tcp::socket(service));


void start_accept(socket_ptr sock) {
	
}

int main() {
	
	cout << "Server started" << endl;
	start_accept(sock);
	service.run();

}