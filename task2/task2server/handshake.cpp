#include "handshake.h"
#include "base64.h"

#include <boost/algorithm/string/predicate.hpp>
#include <openssl/sha.h>
#include <sstream>

using namespace std;

namespace handshake {
	
string computeWSKeyAccept(const string& key) {
  string str(key);
  str += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1((unsigned char *)str.c_str(), str.size(), hash);
  return base64::encode(hash, SHA_DIGEST_LENGTH);
}
	
string createResponse(const string& request) {
  stringstream requestStream(request);
  string key;
  while(requestStream.good()) {
    string line;
    getline(requestStream, line);
    if (boost::starts_with(line, "Sec-WebSocket-Key")) {
      stringstream ss(line);
      ss >> key; // skip the first
      ss >> key;
    }
  }

  stringstream responseStream;
  responseStream << "HTTP/1.1 101 Switching Protocols\r\n";
  responseStream << "Upgrade: websocket\r\n";
  responseStream << "Connection: Upgrade\r\n";
  responseStream << "Sec-WebSocket-Accept: " 
                 << computeWSKeyAccept(key) << "\r\n";
  responseStream << "\r\n";

  return responseStream.str();
}
}