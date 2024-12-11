#include "VMCore.h"

#define MAKE_EXECUTE(method) [this](uint8_t params[], INSTRUCTION instruction) { this->method(params, instruction); }

VMCore::VMCore(VMInstance& newInstance) : VMInstanceBase(newInstance), binaryApi(newInstance), stackApi(newInstance)
{
	opcode_lookup[iPUSH].execute = MAKE_EXECUTE(PUSH);
	opcode_lookup[iPOP].execute = MAKE_EXECUTE(POP);
	opcode_lookup[iDUP].execute = MAKE_EXECUTE(DUP);

	opcode_lookup[iADD].execute = MAKE_EXECUTE(ADD);
	opcode_lookup[iSUB].execute = MAKE_EXECUTE(SUB);
	opcode_lookup[iMUL].execute = MAKE_EXECUTE(MUL);
	opcode_lookup[iDIV].execute = MAKE_EXECUTE(DIV);

	opcode_lookup[iAND].execute = MAKE_EXECUTE(AND);
	opcode_lookup[iOR].execute = MAKE_EXECUTE(OR);
	opcode_lookup[iNOT].execute = MAKE_EXECUTE(NOT);

	opcode_lookup[iEQ].execute = MAKE_EXECUTE(EQ);

	opcode_lookup[iEXIT].execute = MAKE_EXECUTE(EXIT);
}

void VMCore::Parse()
{
	binaryApi.ParseMagic();
	//auto instructionStart = binaryApi.
	for (;;)
	{
		INSTRUCTION instruction = binaryApi.GetInstruction();
		if (instruction.execute == nullptr)
		{
			DIE << "No implementation found for the instruction " << instruction.op_name;
		}
		if (instruction.paramCount > maxParamCount)
		{
			DIE << "instruction: " << instruction.op_name << " Exeeded the max amount of parameters!";
		}

		auto params = binaryApi.GetParams(instruction.paramCount);
		instruction.execute(params, instruction);
	}
}
uint8_t *VMCore::pop()
{
	//auto stackValue = stackApi.Get();
	return stackApi.PopAndGet();
}
void VMCore::push(uint8_t *value, INSTRUCTION &instruction)
{
	stackApi.Push(value, instruction);
	//instance.sp++;
	//instance.stack.push_back(value);
}
void VMCore::PUSH(instuctionParams)
{
	push(params, instruction);
}
void VMCore::POP(instuctionParams)
{
	pop();
}
void VMCore::DUP(instuctionParams)
{
	auto type = stackApi.GetType();
	auto val = stackApi.Get();
	stackApi.PushTyped(val, type);
	//if (instance.sp == 0)
	//{
	//	DIE << "Nothing on the stack to duplicate! At program index " << HEX(instance.pc);
	//}
	//auto val1 = instance.stack.at(instance.sp);
	//push(val1);
}
void VMCore::ADD(instuctionParams)
{
	auto val1 = pop();
	auto val2 = pop();
	push(val1 + val2);
}
void VMCore::SUB(instuctionParams)
{
	auto val1 = pop();
	auto val2 = pop();
	push(val1 - val2);
}
void VMCore::MUL(instuctionParams)
{
	auto val1 = pop();
	auto val2 = pop();
	push(val1 * val2);
}
void VMCore::DIV(instuctionParams)
{
	auto val1 = pop();
	auto val2 = pop();
	push(val1 / val2);
}
void VMCore::AND(instuctionParams)
{
	auto val1 = pop();
	auto val2 = pop();
	push(val1 & val2);
}
void VMCore::OR(instuctionParams)
{
	auto val1 = pop();
	auto val2 = pop();
	push(val1 | val2);
}
void VMCore::NOT(instuctionParams)
{
	auto val1 = pop();
	if (val1 == iFALSE) push(iTRUE);
	else if (val1 == iTRUE) push(iFALSE);
	else DIE << "Stack value expected a bool (0x00 or 0x01) but got " << HEX(val1) << "instead!";
}
void VMCore::EQ(instuctionParams)
{
	auto val1 = pop();
	auto val2 = pop();
	if (val1 == val2)
		push(iTRUE);
	else
		push(iFALSE);
}
void VMCore::EXIT(instuctionParams)
{
	auto type = stackApi.GetType();
	auto val = pop();
	ByteTranslator::Get32(val, type);
	auto exitCode = pop();
	DIE << "Exit was called with code: " << NUM(exitCode);
	//instance.
	exit(exitCode);
}


