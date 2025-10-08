#pragma once

#include <sstream>
#include <iomanip>

class ErrorLogMessage : public std::basic_ostringstream<char>
{
public:
static std::string current_instruction;
	~ErrorLogMessage()
	{
		fflush(stdout);
		fprintf(stderr, "\n\nFatal error on %s: %s\n", current_instruction.c_str(), str().c_str());
		exit(EXIT_FAILURE);
		//std::abort();
	}

};

#define DIE ErrorLogMessage()
#define NUM(val) static_cast<int>(val)
#define HEX(val) "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(val)