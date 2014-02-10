
#include "server.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <cstdlib>
#include <iostream>

#include <string>
#include <sstream>


using boost::asio::ip::tcp;
using namespace std;



int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: server <port>\n";
      return 1;
    }
    
    // TODO: move it to server
    boost::asio::io_service io_service;
    
    // TODO: use lexical_cast
    using namespace std; // For atoi.
    server s(io_service, atoi(argv[1]));
    io_service.run();
    
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}