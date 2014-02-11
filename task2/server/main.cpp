#include "game.h"
#include <boost/lexical_cast.hpp>

#include <stdexcept>
#include <iostream>

using namespace std;
int main(int argc, char* argv[]) {
  try {
    
    if (argc < 2) {
      cerr << "Usage: server <port>\n";
      return 1;
    }
    short port = boost::lexical_cast<short>(argv[1]);
    
    size_t workersNumber = 2;
    
    if (argc == 3) {
      workersNumber = boost::lexical_cast<size_t>(argv[2]);  
      if(workersNumber == 0) {
        cout << "workersNumber must be at least 1" << endl;
        return 1;
      }
    }

    game g(port, workersNumber);
    
  } catch (exception& e) {
    cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}