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
	bool typed_Peek_stack(varible* output, int offset = 0);
	bool typed_StackTruthy();
	bool typed_StackEquals(varible param_value);
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


	void syscall(uint32_t index);


	void typed_setVar(varible reference, varible newValue);
	bool Compare(uint8_t compMode, varible var1, varible var2);
	void Jump(varible offset);
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


#pragma endregion

#pragma region Syscalls
	void SYS_CLEAR_CONSOLE();
	void SYS_READ();
	void SYS_PRINT();
	void SYS_PRINTLN();
	void SYS_DUMP();

	void SYS_PARSE();

	void SYS_INPUT_MODE_READ();
	void SYS_INPUT_MODE_WRITE();
#pragma endregion
#pragma region Syscall_helpers

	void print_raw(varible var);
	void print_raw(uint8_t* data, uint32_t length);
	varible read_raw();
#pragma endregion


};