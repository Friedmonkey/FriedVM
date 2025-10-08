#pragma once

#include <string>
#include "VMInstance.h"
#include "FBinary.h"
#include "InputManager.h"

class VMCore : VMInstanceBase
{
public:
	VMCore(VMInstance& newInstance);
	void Parse();
	void Run(uint64_t start, uint64_t end);
	void AddGlobalVarible(varible var);
	void AddGlobalSymbol(std::string str);
	bool typed_Peek_stack(varible* output, int offset = 0);
	bool Peek_stack(uint32_t *pValue, int offset = 0);
	bool typed_StackTruthy();
	bool typed_StackEquals(varible param_value);
	bool StackEquals(Value& param_value);
private:
	FBinary binaryApi;
	InputManager inputManager;
	varible constTrue;
	varible constFalse;

	uint8_t caseCompareMode = cmEQ;
	varible caseValue = constFalse;

	varible resolveInterpolated(varible interpolated_string);

	varible typed_pop();
	void typed_push(varible value);

	template<typename T>
	T safe_cast(varible var);

	varible dup(varible value);

	void freeVarible(uint32_t index);
	uint32_t pop();
	void push(uint32_t value, bool immediate = true, uint8_t arg_size = 4);
	uint32_t buffer_pop();
	void buffer_push(Value value);
	void syscall(uint32_t index);
	Value makeValue(uint32_t value, bool immediate, uint8_t arg_size);
	void checkVaribleIndex(uint32_t index);
	void GetStructCache(Value& struct_instance, StructCache* cache);
	void GetStructFieldDetails(Value &struct_instance, uint32_t field_index, uint32_t *field_offset, uint8_t *field_length);
	StructCache CacheStructDefinition(uint32_t index);
	bool getStackType(bool* immediate, uint8_t* arg_size);
	uint32_t makeUint(uint32_t value, bool immediate, uint8_t arg_size);
	uint32_t getUintVar();
	Value getVar();
	void setVar(Value reference, Value newValue);
	void typed_setVar(varible reference, varible newValue);
	bool Compare(uint8_t compMode, varible var1, varible var2);
	void Jump(varible offset);
	void Jump(uint32_t offset, bool immidiate);
	void Call(varible offset);
	void Return();
#pragma region typed_Instructions
	bool typed_EXIT(varible* params);
	bool typed_SYSCALL(varible* params);

	bool typed_PUSH(varible* params);
	bool typed_STORE(varible* params);
	bool typed_SET_VAR(varible* params);
	bool typed_POP(varible* params);
	bool typed_SWAP(varible* params);
	bool typed_DUP(varible* params);

	bool typed_MATH(varible* params);
	bool typed_INC(varible* params);
	bool typed_DEC(varible* params);

	//bool typed_AND(varible* params);
	//bool typed_OR(varible* params);
		 
	bool typed_COMP(varible* params);
	bool typed_CHECK_STACK(varible* params);
	bool typed_NOT(varible* params);

	bool typed_JUMP(varible* params);
	bool typed_JUMP_IF(varible* params);
	bool typed_JUMP_IF_STACK(varible* params);

	bool typed_SET_CASE(varible* params);
	bool typed_SET_CASE_MODE(varible* params);
	bool typed_JUMP_IF_CASE(varible* params);

	bool typed_CALL(varible* params);
	bool typed_CALL_IF(varible* params);
	bool typed_CALL_IF_STACK(varible* params);

	bool typed_RET(varible* params);




#pragma endregion
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
	void CALL_IF(uint32_t* params, bool immediate, uint8_t arg_size);

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
	void SYS_PRINTLN();
	void SYS_DUMP();

	//void SYS_TO_STRING_UNSIGNED();
	void SYS_TO_STRING_SIGNED();
	void SYS_PARSE();
	//void SYS_TO_NUMBER_UNSIGNED();
	void SYS_TO_NUMBER_SIGNED();

	void SYS_INPUT_MODE_READ();
	void SYS_INPUT_MODE_WRITE();
	void SYS_INPUT_TO_STRUCT();

	void SYS_SET_CONSOLE_CURSOR();
	void SYS_GET_CONSOLE_CURSOR();
#pragma endregion
#pragma region Syscall_helpers
	void print_raw(Value val);
	void print_raw(varible var);
	void print_raw(uint8_t* data, uint32_t length);
	varible read_raw();
#pragma endregion


};