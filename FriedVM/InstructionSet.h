#pragma once

#include <vector>
#include <string>

struct INSTRUCTION
{
public:
	INSTRUCTION(uint8_t byte, std::string name, uint8_t pcount)
		:opcode(byte), op_name(name), params(pcount) { }
	const uint8_t opcode;
	const std::string op_name;
	const uint8_t params;

};

//magic = FXE1
uint8_t file_magic[] = { 0x46, 0x58, 0x45, 0x31, };
std::vector<INSTRUCTION> op_code_lookup = 
{
	{0x00, "PUSH",		1},
	{0x01, "POP",		0},
	{0x02, "DUP",		0},
	{0x03, "VAR",		2},
	{0x04, "GET_VAR",	1},
	{0x05, "POP_VAR",	1},
	{0x06, "PSH_VAR",	1},
	{0x07, "MOV_VAR",	2},
	{0x08, "DEL",		1},
	{0x09, "ADD",		0},
	{0x0A, "SUB",		0},
	{0x0B, "MUL",		0},
	{0x0C, "DIV",		0},
	{0x0D, "AND",		0},
	{0x0E, "OR",		0},
	{0x0F, "NOT",		0},
	{0x00, "EQ",		0},
	{0x11, "NEQ",		0},
	{0x12, "GT",		0},
	{0x13, "LS",		0},
	{0x14, "JMP",		1},
	{0x15, "JMP_IF",	1},
	{0x16, "CALL",		1},
	{0x17, "CALL_IF",	1},
	{0x18, "RET",		0},
	{0x19, "SYSCALL",	1},
	{0x1A, "EXIT",		0},
};
