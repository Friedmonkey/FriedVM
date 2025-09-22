#pragma once

#include <vector>
#include <string>
#include <functional>
#include "VMInstance.h"

#define NULL_INSTRUCTION INSTRUCTION(0, "null", 0)

struct INSTRUCTION
{
public:
	INSTRUCTION(uint8_t byte, std::string name, uint8_t pcount)
		:opcode(byte), op_name(name), paramCount(pcount), execute(nullptr)
	{
		//execute = nullptr;
		//arg_size = 1;
		//immediate = false;
	}
	const uint8_t opcode;
	const std::string op_name;
	const uint8_t paramCount;
	//uint8_t arg_size; // Argument size (0–3)
	//bool immediate;   // Immediate flag
	//std::function<void(uint32_t* params, bool immediate, uint8_t arg_size)> execute;
	std::function<bool(varible *params)> execute;
	//void (*execute)(uint8_t params[]);
};

struct Statement
{
public:
	Statement(const INSTRUCTION& instruction, varible* parameters)
		: Instruction(instruction), params(parameters)
	{
	}

	INSTRUCTION Instruction;
	varible* params;
};

#define iEXIT		0x0E
#define iSYSCALL	0x0F

#define iPUSH		0x10
#define iSTORE		0x11
#define iSET_VAR	0x12
#define iPOP		0x13
#define iSWAP		0x14
#define iDUP		0x15

#define iMATH		0x30
#define iINC		0x31
#define iDEC		0x32


#define iCOMP			0x40
#define iCHECK_STACK	0x41
#define iNOT			0x42

#define iJUMP			0x50
#define iJUMP_IF		0x51
#define iJUMP_IF_STACK	0x52

#define iCALL			0x60
#define iCALL_IF		0x61
#define iCALL_IF_STACK	0x62


#define iRET			0x6F


#define iSET_BUFFER		0xB0
#define iPUSH_BUFFER	0xB1
#define iGET_BUFFER		0xB2

#define iBUFFER_UTIL	0xB9


#define iSET_STRUCT		0xBA
#define iGET_STRUCT		0xBB
#define iCREATE_STRUCT  0xBC

//#define i			0x18
//#define i			0x19
//#define i			0x1A
//#define i			0x1B
//#define i			0x1C
//#define i			0x1D
//#define i			0x1E
//#define i			0x1F


//math Modes
#define mmADD 0x00
#define mmSUB 0x01
#define mmINC 0x02
#define mmDEC 0x03
#define mmMUL 0x04
#define mmDIV 0x05
#define mmPOW 0x06
#define mmROOT 0x07
#define mmSQRT 0x08
#define mmRAND 0x09

//comp Modes
#define cmGT 0x00
#define cmGTE 0x01
#define cmLT 0x02
#define cmLTE 0x03
#define cmEQ 0x04
#define cmNEQ 0x05

//buffer Modes
#define bmCLEAR				0x00
#define bmPOP_TO_STACK		0x01
#define bmPUSH_FROM_STACK	0x02
#define bmREMOVE_FROM_END	0x03
#define bmFORMAT			0x04
//#define bm					0x05

#define iTRUE		0x01
#define iFALSE		0x00

const uint8_t structIndexByteCount = 4;
const uint8_t maxParamCount = 2;
//magic = FXE
const uint8_t file_magic[] = { 0x46, 0x58, 0x45 };
const uint8_t symbolSplitCar = 0xBB;
extern std::vector<INSTRUCTION> opcode_lookup;
extern std::vector<std::function<void()>> syscall_lookup;

