#pragma once 

// I took some code from http://katzarsky.github.com/WebSocket

#include <string>

namespace websocketTools {
	
enum WebSocketFrameType {
	ERROR_FRAME             = 0xFF00,
	INCOMPLETE_FRAME        = 0xFE00,

	OPENING_FRAME           = 0x3300,
	CLOSING_FRAME           = 0x3400,

	INCOMPLETE_TEXT_FRAME   = 0x01,
	INCOMPLETE_BINARY_FRAME = 0x02,

	TEXT_FRAME              = 0x81,
	BINARY_FRAME            = 0x82,

	PING_FRAME              = 0x19,
	PONG_FRAME              = 0x1A
};
	
std::string createHandshakeResponseHeader(
	const std::string& request
);

WebSocketFrameType getMessage(
	char* data,
	size_t length,
	std::string& res
);

WebSocketFrameType getFrame(
	unsigned char* in_buffer, 
	int in_length, 
	unsigned char* out_buffer, 
	int out_size, 
	int* out_length
);

size_t createMassage(
	const std::string& message,
	char* buffer
);

int createFrame(
	WebSocketFrameType frame_type,
	unsigned char* msg,
	int msg_length,
	unsigned char* buffer
);

}