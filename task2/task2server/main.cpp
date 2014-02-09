#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <algorithm>
#include <string>

using namespace std;
using namespace boost;
using namespace boost::asio;

typedef shared_ptr<ip::tcp::socket> socket_ptr;

void start_accept(socket_ptr);
void handle_accept(socket_ptr, const system::error_code&);

io_service service;
ip::tcp::endpoint ep(ip::tcp::v4(), 2014);
ip::tcp::acceptor acc(service, ep);
socket_ptr sock(new ip::tcp::socket(service));

void start_accept(socket_ptr sock) {
	acc.async_accept(*sock, boost::bind(handle_accept, sock, _1));
}

void handle_accept(socket_ptr sock, const system::error_code& err) {
	if (err) {
		cout << "Handle accept error" << endl;
		return;
	}
	cout << "Got new connect!" << endl;
	socket_ptr next_sock(new ip::tcp::socket(service));
	start_accept(next_sock);
}

size_t read_complete(char* buff, const system::error_code& err, 
				   size_t bytes) {
	if(err) return 0;
	bool found = std::find(buff, buff + bytes, '\n') < 
				 buff + bytes;
	return found ? 0 : 1;
}

int main() {
	cout << "Server started" << endl;
	start_accept(sock);
	service.run();
	cout << "Server stopped" << endl;
}