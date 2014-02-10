#pragma once

#include <string>

namespace base64 {
	
std::string encode(
	unsigned char const* bytes_to_encode, 
	unsigned int len
);

std::string decode(
	std::string const& s
);

}
