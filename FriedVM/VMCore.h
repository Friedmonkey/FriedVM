#pragma once

#include <string>
#include "VMInstance.h"
#include "FBinary.h"

class VMCore : VMInstanceBase
{
public:
	VMCore(VMInstance& newInstance);
	void Parse();
	void Run(uint64_t start, uint64_t end);
	bool Peek_stack(uint32_t *pValue, int offset = 0);
	bool StackEquals(Value& param_value);
private:
	FBinary binaryApi;
	//FStack stackApi;
	void freeVarible(uint32_t index);
	uint32_t pop();
	void push(uint32_t value, bool immediate = true, uint8_t arg_size = 4);
	uint32_t buffer_pop();
	void buffer_push(Value value);
	void syscall(uint32_t index);
	Value makeValue(uint32_t value, bool immediate, uint8_t arg_size);
	void checkVaribleIndex(uint32_t index);
	void GetStructFieldDetails(Value &struct_instance, uint32_t field_index, uint32_t *field_offset, uint8_t *field_length);
	StructCache CacheStructDefinition(uint32_t index);
	bool getStackType(bool* immediate, uint8_t* arg_size);
	uint32_t getUintVar();
	Value getVar();
	void setVar(Value reference, Value newValue);
	void Jump(uint32_t offset, bool immidiate);
	void Call(uint32_t offset, bool immidiate);
	void Return();
#pragma region Instructions
	void PUSH(uint32_t* params, bool immediate, uint8_t arg_size);
	void POP(uint32_t* params, bool immediate, uint8_t arg_size);
	void DUP(uint32_t* params, bool immediate, uint8_t arg_size);

	void MATH(uint32_t* params, bool immediate, uint8_t arg_size);

	void AND(uint32_t* params, bool immediate, uint8_t arg_size);
	void OR(uint32_t* params, bool immediate, uint8_t arg_size);
	void NOT(uint32_t* params, bool immediate, uint8_t arg_size);
	
	void COMP(uint32_t* params, bool immediate, uint8_t arg_size);
	
	void JUMP(uint32_t* params, bool immediate, uint8_t arg_size);
	void JUMP_IF(uint32_t* params, bool immediate, uint8_t arg_size);

	void CALL(uint32_t* params, bool immediate, uint8_t arg_size);
	//void CALL_IF(uint32_t* params, bool immediate, uint8_t arg_size);

	void RET(uint32_t* params, bool immediate, uint8_t arg_size);
	//
	void SYSCALL(uint32_t* params, bool immediate, uint8_t arg_size);
	
	void EXIT(uint32_t* params, bool immediate, uint8_t arg_size);

	void SET_BUFFER(uint32_t* params, bool immediate, uint8_t arg_size);
	void GET_BUFFER(uint32_t* params, bool immediate, uint8_t arg_size);
	void PUSH_BUFFER(uint32_t* params, bool immediate, uint8_t arg_size);
	void BUFFER_UTIL(uint32_t* params, bool immediate, uint8_t arg_size);
	void SET_VAR(uint32_t* params, bool immediate, uint8_t arg_size);
	void SET_STRUCT(uint32_t* params, bool immediate, uint8_t arg_size);
	void GET_STRUCT(uint32_t* params, bool immediate, uint8_t arg_size);
	void CREATE_STRUCT(uint32_t* params, bool immediate, uint8_t arg_size);
	
	void CHECK_STACK(uint32_t* params, bool immediate, uint8_t arg_size);
	//void GET_VAR(uint32_t* params, bool immediate, uint8_t arg_size);
	//void POP_VAR(uint32_t* params, bool immediate, uint8_t arg_size);
	//void PSH_VAR(uint32_t* params, bool immediate, uint8_t arg_size);
	//void MOV_VAR(uint32_t* params, bool immediate, uint8_t arg_size);
	//void DEL(uint32_t* params, bool immediate, uint8_t arg_size);
	//void DEL(uint32_t* params, bool immediate, uint8_t arg_size);
	//void DEL(uint32_t* params, bool immediate, uint8_t arg_size);
	//void DEL(uint32_t* params, bool immediate, uint8_t arg_size);
#pragma endregion

#pragma region Syscalls
	void SYS_PAUSE();
	void SYS_CLEAR_CONSOLE();
	void SYS_READ();
	void SYS_PRINT();
	void SYS_DUMP();
	void SYS_TO_STRING_UNSIGNED();
	void SYS_TO_STRING_SIGNED();
	void SYS_TO_NUMBER_UNSIGNED();
	void SYS_TO_NUMBER_SIGNED();
	void SYS_INPUT_TO_STRUCT();
#pragma endregion
#pragma region Syscall_helpers
	void print_raw(Value val);
	void print_raw(uint8_t* data, uint32_t length);
	Value read_raw();
#pragma endregion


};