#pragma once

#include <vector>
#include <cstdint>
#include "Logger.h"

struct VMInstance
{
public:
	uint8_t pc = 0;
	std::vector<uint8_t> bytecode;
};

class VMInstanceBase
{
public:
	VMInstanceBase(VMInstance& newInstance) : instance(newInstance)
	{
	}
	VMInstance& instance;
};