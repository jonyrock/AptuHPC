
#include "server.h"

#include <boost/lexical_cast.hpp>

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
    
    short port = boost::lexical_cast<short>(argv[1]);
    server s(port);
    
    
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}