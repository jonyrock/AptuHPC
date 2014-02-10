#include "game.h"
#include <boost/lexical_cast.hpp>

#include <stdexcept>
#include <iostream>

using namespace std;
int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      cerr << "Usage: server <port>\n";
      return 1;
    }
    
    short port = boost::lexical_cast<short>(argv[1]);
    game g(port, 10);
    
  } catch (exception& e) {
    cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}