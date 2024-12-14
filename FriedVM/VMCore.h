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
	uint32_t pop();
	void push(uint32_t value);
	void syscall(uint32_t index);
#pragma region Instructions
	void PUSH(uint32_t params[]);
	void POP(uint32_t params[]);
	void DUP(uint32_t params[]);

	//void VAR(uint32_t params[]);
	//void GET_VAR(uint32_t params[]);
	//void POP_VAR(uint32_t params[]);
	//void PSH_VAR(uint32_t params[]);
	//void MOV_VAR(uint32_t params[]);
	//void DEL(uint32_t params[]);

	void ADD(uint32_t params[]);
	void SUB(uint32_t params[]);
	void MUL(uint32_t params[]);
	void DIV(uint32_t params[]);
	
	void AND(uint32_t params[]);
	void OR(uint32_t params[]);
	void NOT(uint32_t params[]);
	
	void EQ(uint32_t params[]);
	void NEQ(uint32_t params[]);
	void GT(uint32_t params[]);
	void GTEQ(uint32_t params[]);
	void LT(uint32_t params[]);
	void LTEQ(uint32_t params[]);
	
	//void JMP(uint32_t params[]);
	//void JMP_IF(uint32_t params[]);

	//void CALL(uint32_t params[]);
	//void CALL_IF(uint32_t params[]);

	//void RET(uint32_t params[]);
	//
	void SYSCALL(uint32_t params[]);
	
	void EXIT(uint32_t params[]);
#pragma endregion

#pragma region Syscalls
	void SYS_PAUSE();
	void SYS_CLEAR();
	void SYS_READ();
	void SYS_PRINT();
#pragma endregion
#pragma region Syscall_helpers
	void print_raw(uint8_t* data, uint32_t length);
#pragma endregion


};