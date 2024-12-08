#pragma once

#include <vector>
#include <string>
#include <functional>

#define NULL_INSTRUCTION INSTRUCTION(0, "null", 0)
struct INSTRUCTION
{
public:
	INSTRUCTION(uint8_t byte, std::string name, uint8_t pcount)
		:opcode(byte), op_name(name), paramCount(pcount) 
	{
		execute = nullptr;
	}
	const uint8_t opcode;
	const std::string op_name;
	const uint8_t paramCount;
	std::function<void(uint8_t[])> execute;
	//void (*execute)(uint8_t params[]);
};

const uint8_t maxParamCount = 2;
//magic = FXE1
const uint8_t file_magic[] = { 0x46, 0x58, 0x45, 0x31, };
extern std::vector<INSTRUCTION> opcode_lookup;

