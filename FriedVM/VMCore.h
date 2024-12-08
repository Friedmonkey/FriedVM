#pragma once

#include <vector>
#include <cstdint>
#include "InstructionSet.h"

class VMCore
{
public:
	VMCore(uint8_t programCounter = 0);
	bool ReadMagic();

private:
	uint8_t pc;
	std::vector<uint8_t> bytecode;
};