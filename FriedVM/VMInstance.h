#pragma once

#include <vector>
#include <cstdint>
#include "Logger.h"

struct VMInstance
{
public:
	uint64_t pc = 0;
	std::vector<uint8_t> bytecode;

	std::vector<uint64_t> call_stack;

	uint8_t sp = 0;
	std::vector<uint32_t> stack;
	std::vector<uint8_t> stack_type;

	std::vector<uint32_t> meta;
	std::vector<uint8_t*> varibles;
	uint32_t declare_size = 0;

	std::vector<uint8_t> varible_buffer;

	uint64_t instructionStart = 0;
	uint8_t header_size = 8;
	uint8_t meta_size = 4;
	uint8_t emptyVar_size = 2;
	uint8_t version = 1;

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