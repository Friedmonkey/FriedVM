#pragma once

#include <vector>
#include <cstdint>
#include "Logger.h"

struct VMInstance
{
public:
	uint64_t pc = 0;
	std::vector<uint8_t> bytecode;

	uint8_t sp = 0;
	std::vector<uint32_t> stack;

	std::vector<uint32_t> meta;
	std::vector<uint8_t*> varibles;

	uint8_t version = 1;
	uint8_t header_size = 8;
	uint8_t meta_size = 4;

	bool hasSymbols;
	std::vector<uint8_t> symbols_length;
	std::vector<uint8_t*> symbols;
};

class VMInstanceBase
{
public:
	VMInstanceBase(VMInstance& newInstance) : instance(newInstance)
	{
	}
	VMInstance& instance;
};