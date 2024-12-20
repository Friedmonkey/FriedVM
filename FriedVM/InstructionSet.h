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
	uint8_t arg_size; // Argument size (0�3)
	bool immediate;   // Immediate flag
	std::function<void(uint32_t* params, bool immediate, uint8_t arg_size)> execute;
	//void (*execute)(uint8_t params[]);
};

//struct OpcodeArguments
//{
//	uint32_t *params;
//	bool immediate;
//	uint8_t arg_size;
//};

struct Value
{
public:
	Value(uint8_t *pData, uint32_t mLength, bool mImmediate = true, uint32_t mIndex = 0) : data(pData), length(mLength), immediate(mImmediate), index(mIndex)
	{

	}
	uint8_t *data;
	uint32_t length;
	uint32_t index;
	bool immediate;
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
#define iMATH		0x09
#define iAND		0x0B
#define iOR			0x0C
#define iNOT		0x0D
#define iCOMP		0x0A

#define iJUMP		0x0E
#define iJUMP_IF	0x0F
#define iCALL		0x10
#define iCALL_IF	0x11
#define iRET		0x12
#define iSYSCALL	0x13
#define iEXIT		0x14
//#define i			0x15
//#define i			0x16
//#define i			0x17
//#define i			0x18
//#define i			0x19
//#define i			0x1A
//#define i			0x1B
//#define i			0x1C
//
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

//comp Modes
#define cmGT 0x00
#define cmGTE 0x01
#define cmLT 0x02
#define cmLTE 0x03
#define cmEQ 0x04
#define cmNEQ 0x05

#define iTRUE		0x01
#define iFALSE		0x00

const uint8_t maxParamCount = 2;
//magic = FXE
const uint8_t file_magic[] = { 0x46, 0x58, 0x45 };
const uint8_t symbolSplitCar = 0xBB;
extern std::vector<INSTRUCTION> opcode_lookup;
extern std::vector<std::function<void()>> syscall_lookup;

