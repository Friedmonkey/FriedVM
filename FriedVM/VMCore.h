#pragma once

#include <string>
#include "VMInstance.h"
#include "FBinary.h"

class VMCore : VMInstanceBase
{
public:
	VMCore(VMInstance& newInstance);
	void Parse();
	bool Peek_stack(uint32_t &rValue, int offset = 0);
private:
	FBinary binaryApi;
	//FStack stackApi;
	uint32_t pop();
	void push(uint32_t value, bool immediate = true, uint8_t arg_size = 1);
	void syscall(uint32_t index);
	Value getVar();
	void Jump(uint32_t offset, bool immidiate);
#pragma region Instructions
	void PUSH(uint32_t* params, bool immediate, uint8_t arg_size);
	void POP(uint32_t* params, bool immediate, uint8_t arg_size);
	void DUP(uint32_t* params, bool immediate, uint8_t arg_size);

	//void VAR(uint32_t* params, bool immediate, uint8_t arg_size);
	//void GET_VAR(uint32_t* params, bool immediate, uint8_t arg_size);
	//void POP_VAR(uint32_t* params, bool immediate, uint8_t arg_size);
	//void PSH_VAR(uint32_t* params, bool immediate, uint8_t arg_size);
	//void MOV_VAR(uint32_t* params, bool immediate, uint8_t arg_size);
	//void DEL(uint32_t* params, bool immediate, uint8_t arg_size);

	void ADD(uint32_t* params, bool immediate, uint8_t arg_size);
	void SUB(uint32_t* params, bool immediate, uint8_t arg_size);
	void MUL(uint32_t* params, bool immediate, uint8_t arg_size);
	void DIV(uint32_t* params, bool immediate, uint8_t arg_size);

	void INC(uint32_t* params, bool immediate, uint8_t arg_size);
	void DEC(uint32_t* params, bool immediate, uint8_t arg_size);

	
	void AND(uint32_t* params, bool immediate, uint8_t arg_size);
	void OR(uint32_t* params, bool immediate, uint8_t arg_size);
	void NOT(uint32_t* params, bool immediate, uint8_t arg_size);
	
	void EQ(uint32_t* params, bool immediate, uint8_t arg_size);
	void NEQ(uint32_t* params, bool immediate, uint8_t arg_size);
	void GT(uint32_t* params, bool immediate, uint8_t arg_size);
	void GTEQ(uint32_t* params, bool immediate, uint8_t arg_size);
	void LT(uint32_t* params, bool immediate, uint8_t arg_size);
	void LTEQ(uint32_t* params, bool immediate, uint8_t arg_size);
	
	void JUMP(uint32_t* params, bool immediate, uint8_t arg_size);
	void JUMP_IF(uint32_t* params, bool immediate, uint8_t arg_size);
	void JUMP_IF_STACK(uint32_t* params, bool immediate, uint8_t arg_size);

	//void CALL(uint32_t* params, bool immediate, uint8_t arg_size);
	//void CALL_IF(uint32_t* params, bool immediate, uint8_t arg_size);

	//void RET(uint32_t* params, bool immediate, uint8_t arg_size);
	//
	void SYSCALL(uint32_t* params, bool immediate, uint8_t arg_size);
	
	void EXIT(uint32_t* params, bool immediate, uint8_t arg_size);
#pragma endregion

#pragma region Syscalls
	void SYS_PAUSE();
	void SYS_CLEAR();
	void SYS_READ();
	void SYS_PRINT();
	void SYS_DUMP();
#pragma endregion
#pragma region Syscall_helpers
	void print_raw(Value val);
	void print_raw(uint8_t* data, uint32_t length);
#pragma endregion


};