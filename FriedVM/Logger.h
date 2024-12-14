#pragma once

#include <sstream>
#include <iomanip>

class ErrorLogMessage : public std::basic_ostringstream<char>
{
public:
	~ErrorLogMessage()
	{
		fprintf(stderr, "\n\nFatal error: %s\n", str().c_str());
		exit(EXIT_FAILURE);
	}

};

#define DIE ErrorLogMessage()
#define NUM(val) static_cast<int>(val)
#define HEX(val) "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(val)