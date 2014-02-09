#include "websocketTools.h"
#include "base64.h"

#include <boost/algorithm/string/predicate.hpp>
#include <openssl/sha.h>
#include <sstream>

#include <assert.h>
#include <stdint.h> /* uint8_t */
#include <stdio.h>  /* sscanf */
#include <ctype.h>  /* isdigit */
#include <stddef.h> /* int */

using namespace std;

namespace websocketTools {
	
string computeWSKeyAccept(const string& key) {
  string str(key);
  str += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1((unsigned char *)str.c_str(), str.size(), hash);
  return base64::encode(hash, SHA_DIGEST_LENGTH);
}

string createHandshakeResponseHeader(const string& request) {
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

WebSocketFrameType getMessage(char * data, size_t length, std::string& res) {
  unsigned char* decodedFrameData = new unsigned char[1024];
  int decodedLength = 0;
  auto type = websocketTools::getFrame(
    (unsigned char *)data,
    length,
    decodedFrameData,
    1024,
    &decodedLength
  );
  res.append((const char*)decodedFrameData, decodedLength);
  delete [] decodedFrameData;
  return type;
}
  
WebSocketFrameType getFrame(
  unsigned char* in_buffer, 
  int in_length,
  unsigned char* out_buffer,
  int out_size,
  int* out_length
) {

  //printf("getTextFrame()\n");
  if(in_length < 3) return INCOMPLETE_FRAME;

  unsigned char msg_opcode = in_buffer[0] & 0x0F;
  unsigned char msg_fin = (in_buffer[0] >> 7) & 0x01;
  unsigned char msg_masked = (in_buffer[1] >> 7) & 0x01;

  // *** message decoding
  int payload_length = 0;
  int pos = 2;
  int length_field = in_buffer[1] & (~0x80);
  unsigned int mask = 0;

  //printf("IN:"); for(int i=0; i<20; i++) printf("%02x ",buffer[i]); printf("\n");

  if(length_field <= 125) {
    payload_length = length_field;
  }
  else if(length_field == 126) { //msglen is 16bit!
    payload_length = in_buffer[2] + (in_buffer[3]<<8);
    pos += 2;
  }
  else if(length_field == 127) { //msglen is 64bit!
    payload_length = in_buffer[2] + (in_buffer[3]<<8); 
    pos += 8;
  }
  //printf("PAYLOAD_LEN: %08x\n", payload_length);
  if(in_length < payload_length+pos) {
    return INCOMPLETE_FRAME;
  }

  if(msg_masked) {
    mask = *((unsigned int*)(in_buffer+pos));
    //printf("MASK: %08x\n", mask);
    pos += 4;

    // unmask data:
    unsigned char* c = in_buffer+pos;
    for(int i=0; i<payload_length; i++) {
      c[i] = c[i] ^ ((unsigned char*)(&mask))[i%4];
    }
  }
  
  if(payload_length > out_size) {
    //TODO: if output buffer is too small -- ERROR or resize(free and allocate bigger one) the buffer ?
  }

  memcpy((void*)out_buffer, (void*)(in_buffer+pos), payload_length);
  out_buffer[payload_length] = 0;
  *out_length = payload_length+1;
  
  //printf("TEXT: %s\n", out_buffer);

  if(msg_opcode == 0x0) return (msg_fin)?TEXT_FRAME:INCOMPLETE_TEXT_FRAME; // continuation frame ?
  if(msg_opcode == 0x1) return (msg_fin)?TEXT_FRAME:INCOMPLETE_TEXT_FRAME;
  if(msg_opcode == 0x2) return (msg_fin)?BINARY_FRAME:INCOMPLETE_BINARY_FRAME;
  if(msg_opcode == 0x9) return PING_FRAME;
  if(msg_opcode == 0xA) return PONG_FRAME;

  return ERROR_FRAME;
}

size_t createMassage(const string& message, char* buffer) {
  return (size_t)createFrame(
      websocketTools::TEXT_FRAME,
      (unsigned char*)message.c_str(),
      message.size(),
      (unsigned char*)buffer
  );
}

int createFrame(
  WebSocketFrameType frame_type, 
  unsigned char* msg, int msg_length, unsigned char* buffer
) {
  
  int pos = 0;
  int size = msg_length; 
  buffer[pos++] = (unsigned char)frame_type; // text frame

  if(size<=125) {
    buffer[pos++] = size;
  }
  else if(size<=65535) {
    buffer[pos++] = 126; //16 bit length
    buffer[pos++] = (size >> 8) & 0xFF; // rightmost first
    buffer[pos++] = size & 0xFF;
  }
  else { // >2^16-1
    buffer[pos++] = 127; //64 bit length
    
    //TODO: write 8 bytes length
    pos+=8;
  }
  memcpy((void*)(buffer+pos), msg, size);
  return (size+pos);
}

}