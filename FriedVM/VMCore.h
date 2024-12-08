#pragma once

#include <string>
#include "VMInstance.h"
#include "FBinary.h"

class VMCore : VMInstanceBase
{
public:
	VMCore(VMInstance& newInstance);
	void Parse();
private:
	FBinary binaryApi;
	//FStack stackApi;
	uint8_t pop();
	void push(uint8_t value);

	void PUSH(uint8_t params[]);
	void POP(uint8_t params[]);
	void DUP(uint8_t params[]);

	//void VAR(uint8_t params[]);
	//void GET_VAR(uint8_t params[]);
	//void POP_VAR(uint8_t params[]);
	//void PSH_VAR(uint8_t params[]);
	//void MOV_VAR(uint8_t params[]);
	//void DEL(uint8_t params[]);

	void ADD(uint8_t params[]);
	void SUB(uint8_t params[]);
	void MUL(uint8_t params[]);
	void DIV(uint8_t params[]);
	
	void AND(uint8_t params[]);
	void OR(uint8_t params[]);
	void NOT(uint8_t params[]);
	
	void EQ(uint8_t params[]);
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
	
	void EXIT(uint8_t params[]);
};