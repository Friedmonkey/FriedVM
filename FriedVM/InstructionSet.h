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
		arg_size = 1;
		immediate = false;
	}
	const uint8_t opcode;
	const std::string op_name;
	const uint8_t paramCount;
	uint8_t arg_size; // Argument size (0–3)
	bool immediate;   // Immediate flag
	std::function<void(uint32_t[])> execute;
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
#define iGTEQ		0x13
#define iLT			0x14
#define iLTEQ		0x15
#define iJMP		0x16
#define iJMP_IF		0x17
#define iCALL		0x18
#define iCALL_IF	0x19
#define iRET		0x1A
#define iSYSCALL	0x1B
#define iEXIT		0x1C


#define iTRUE		0x01
#define iFALSE		0x00

const uint8_t maxParamCount = 2;
//magic = FXE
const uint8_t file_magic[] = { 0x46, 0x58, 0x45 };
const uint8_t symbolSplitCar = 0xBB;
extern std::vector<INSTRUCTION> opcode_lookup;
extern std::vector<std::function<void()>> syscall_lookup;

