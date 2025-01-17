#pragma once

#include <vector>
#include <cstdint>
#include "Logger.h"
#include <unordered_map>
#include "Types/BaseValue.h"

struct StructCache {
	std::vector<uint32_t> offsets;
	std::vector<uint8_t> lengths;
	uint32_t total_size = 0;
	uint8_t *initial_data = 0;
};
struct Value
{
public:
	Value(uint8_t* pData, uint32_t mLength, bool mImmediate = true, uint32_t mIndex = 0) : data(pData), length(mLength), immediate(mImmediate), index(mIndex)
	{

	}
	uint8_t* data;

	uint32_t length;
	uint32_t index;
	bool immediate;
};
struct VMInstance
{
public:
	uint64_t pc = 0;
	std::vector<uint8_t> bytecode;

	std::vector<uint64_t> call_stack;

	uint8_t sp = 0;
	std::vector<uint32_t> stack;
	std::vector<uint8_t> stack_type;

	std::vector<BaseValue> declares;

	std::vector<uint32_t> meta;
	std::vector<uint8_t*> varibles;
	uint32_t declare_size = 0;

	std::vector<uint8_t> varible_buffer;

	std::unordered_map<uint32_t, StructCache> structCache;

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