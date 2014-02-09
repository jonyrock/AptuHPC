//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <iostream>
#include <string>
#include <sstream>

#include "base64.h"
#include <openssl/sha.h>

using boost::asio::ip::tcp;
using namespace std;

class session
{
public:
  session(boost::asio::io_service& io_service)
    : socket_(io_service)
  {
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {
  	cout << "start" << endl;
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }
  
  string getWSKeyAccept(const string& key) {
	string str(key);
	str += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	unsigned char hash[SHA_DIGEST_LENGTH];
 	SHA1((unsigned char *)str.c_str(), str.size(), hash);
  	return base64_encode(hash, SHA_DIGEST_LENGTH);
  }
  
  string buildWebSocketResponse(const string& request) {
  	stringstream requestStream(request);
  	string key;
  	while(requestStream.good()) {
  		string line;
  		getline(requestStream, line);
  		if (boost::starts_with(line, "Sec-WebSocket-Key")) {
  			stringstream ss(line);
  			ss >> key; // skip first
  			ss >> key;
  		}
  	}
  	
  	stringstream responseStream;
  	responseStream << "HTTP/1.1 101 Switching Protocols\r\n";
  	responseStream << "Upgrade: websocket\r\n";
  	responseStream << "Connection: Upgrade\r\n";
  	responseStream << "Sec-WebSocket-Accept: " 
  	               << getWSKeyAccept(key) << "\r\n";
  	responseStream << "\r\n";
  	
  	return responseStream.str();
  }

  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred) {
    if (!error) {
    	string str(data_, bytes_transferred);
    	string response = buildWebSocketResponse(str);
    	response.copy(data_, response.size(), 0);
    	data_[response.size()] = '\0';
    	cout << "Client connected" << endl;
      	boost::asio::async_write (
      		socket_,
      		boost::asio::buffer(data_, response.size()),
			boost::bind (
				&session::handle_write, this,
				boost::asio::placeholders::error
			)
        );
    } else {
      delete this;
    }
  }

  void handle_write(const boost::system::error_code& error) {
	if (!error) {
    	cout << "something is written" << endl;
      	socket_.async_read_some(
      		boost::asio::buffer(data_, max_length),
      		boost::bind (
      			&session::handle_read, this,
            	boost::asio::placeholders::error,
            	boost::asio::placeholders::bytes_transferred
            )
      	);
    } else {
    	cout << "write error" << endl;
      delete this;
    }
  }

private:
  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

class server
{
public:
  server(boost::asio::io_service& io_service, short port)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
  {
    session* new_session = new session(io_service_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }

void handle_accept(session* new_session, 
  	                 const boost::system::error_code& error) {
	if (!error) {
      new_session->start();
      new_session = new session(io_service_);
      acceptor_.async_accept(new_session->socket(),
          boost::bind(&server::handle_accept, this, new_session,
            boost::asio::placeholders::error));
    } else {
		delete new_session;
    }
}

private:
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
};

int main(int argc, char* argv[]){
  try {
    if (argc != 2) {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;
    using namespace std; // For atoi.
    server s(io_service, atoi(argv[1]));
    io_service.run();
    
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}