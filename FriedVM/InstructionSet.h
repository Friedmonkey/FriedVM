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

#define iPUSH		0x00
#define iPOP		0x01
#define iDUP		0x02
#define iVAR		0x03
#define iGET_VAR	0x04
#define iPOP_VAR	0x05
#define iPSH_VAR	0x06
#define iMOV_VAR	0x07
#define iDEL		0x08
#define iADD		0x09
#define iSUB		0x0A
#define iMUL		0x0B
#define iDIV		0x0C
#define iAND		0x0D
#define iOR			0x0E
#define iNOT		0x0F
#define iEQ			0x10
#define iNEQ		0x11
#define iGT			0x12
#define iLS			0x13
#define iJMP		0x14
#define iJMP_IF		0x15
#define iCALL		0x16
#define iCALL_IF	0x17
#define iRET		0x18
#define iSYSCALL	0x19
#define iEXIT		0x1A


#define iTRUE		0x01
#define iFALSE		0x00

const uint8_t maxParamCount = 2;
//magic = FXE1
const uint8_t file_magic[] = { 0x46, 0x58, 0x45, 0x31, };
extern std::vector<INSTRUCTION> opcode_lookup;

