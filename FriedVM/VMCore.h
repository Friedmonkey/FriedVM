#pragma once

#include <string>
#include "VMInstance.h"
#include "FBinary.h"
#include "FStack.h"

#define instuctionParams uint8_t params[], INSTRUCTION instruction
#define mPUSH(val) push(val, &instruction)
class VMCore : VMInstanceBase
{
public:
	VMCore(VMInstance& newInstance);
	void Parse();
private:
	FBinary binaryApi;
	FStack stackApi;
	uint8_t *pop();
	void push(uint8_t *value, INSTRUCTION &instruction);

	void PUSH(instuctionParams);
	void POP(instuctionParams);
	void DUP(instuctionParams);

	//void VAR(uint8_t params[]);
	//void GET_VAR(uint8_t params[]);
	//void POP_VAR(uint8_t params[]);
	//void PSH_VAR(uint8_t params[]);
	//void MOV_VAR(uint8_t params[]);
	//void DEL(uint8_t params[]);

	void ADD(instuctionParams);
	void SUB(instuctionParams);
	void MUL(instuctionParams);
	void DIV(instuctionParams);
	
	void AND(instuctionParams);
	void OR(instuctionParams);
	void NOT(instuctionParams);
	
	void EQ(instuctionParams);
	//void NEQ(uint8_t params[]);
	//void GT(uint8_t params[]);
	//void LT(uint8_t params[]);
	
	//void JMP(uint8_t params[]);
	//void JMP_IF(uint8_t params[]);

	//void CALL(uint8_t params[]);
	//void CALL_IF(uint8_t params[]);

	//void RET(uint8_t params[]);
	//
	//void SYSCALL(uint8_t params[]);
	
	void EXIT(instuctionParams);
};