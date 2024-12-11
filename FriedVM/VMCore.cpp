#include "VMCore.h"

#define MAKE_EXECUTE(method) [this](uint32_t params[]) { this->method(params); }

VMCore::VMCore(VMInstance& newInstance) : VMInstanceBase(newInstance), binaryApi(newInstance)
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
	bool hasSymbols = binaryApi.ParseMagic();
	uint64_t instructionStart = binaryApi.ParseAddress();
	uint64_t constPoolStart = binaryApi.ParseAddress();
	uint64_t symbolsStart = 0;
	if (hasSymbols)
	{
		symbolsStart = binaryApi.ParseAddress();
	}
	uint64_t totalLength = 0;
	uint64_t pcBackup = 0;
	while(instance.pc < instructionStart)
	{
		uint32_t length = binaryApi.ParseMeta();

		if (instance.bytecode.size() < (totalLength + length))
		{
			DIE << "file size was too small (" << NUM(instance.bytecode.size()) << "), expected more bytes (" << NUM(instance.pc + count) << ")";
		}
		uint8_t *buffer = new uint8_t[length];
		for (size_t i = 0; i < length; i++)
		{
			buffer[i] = instance.bytecode[totalLength + 1];
		}
		totalLength += length;

		instance.varibles.push_back(buffer);
	}
	//instance.pc = instructionStart;
	while(instance.pc < constPoolStart)
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

		auto params = binaryApi.GetParams(instruction);
		instruction.execute(params);
	}
	if (!hasSymbols || symbolsStart == 0) return;
	//while (instance.pc < instance.bytecode.size())
	//{
	//	std::vector<char> symbolBuffer;
	//	do
	//	{
	//		symbolBuffer.push_back(instance.bytecode[instance.pc]);
	//	}
	//	while (instance.bytecode[instance.pc] != 0xBB);
	//	instance.symbols.push_back(symbolBuffer);
	//	instance.pc++;
	//}
}
uint32_t VMCore::pop()
{
	if (instance.sp == 0)
	{
		DIE << "Nothing on the stack to pop! At program index " << HEX(instance.pc);
	}
	instance.sp--;
	uint32_t value = instance.stack.at(instance.sp);
	instance.stack.pop_back();
	return value;
}
void VMCore::push(uint32_t value)
{
	instance.sp++;
	instance.stack.push_back(value);
}
void VMCore::PUSH(uint32_t params[])
{
	push(params[0]);
}
void VMCore::POP(uint32_t params[])
{
	pop();
}
void VMCore::DUP(uint32_t params[])
{
	if (instance.sp == 0)
	{
		DIE << "Nothing on the stack to duplicate! At program index " << HEX(instance.pc);
	}
	auto val1 = instance.stack.at(instance.sp);
	push(val1);
}
void VMCore::ADD(uint32_t params[])
{
	auto val1 = pop();
	auto val2 = pop();
	push(val1 + val2);
}
void VMCore::SUB(uint32_t params[])
{
	auto val1 = pop();
	auto val2 = pop();
	push(val1 - val2);
}
void VMCore::MUL(uint32_t params[])
{
	auto val1 = pop();
	auto val2 = pop();
	push(val1 * val2);
}
void VMCore::DIV(uint32_t params[])
{
	auto val1 = pop();
	auto val2 = pop();
	push(val1 / val2);
}
void VMCore::AND(uint32_t params[])
{
	auto val1 = pop();
	auto val2 = pop();
	push(val1 & val2);
}
void VMCore::OR(uint32_t params[])
{
	auto val1 = pop();
	auto val2 = pop();
	push(val1 | val2);
}
void VMCore::NOT(uint32_t params[])
{
	auto val1 = pop();
	if (val1 == iFALSE) push(iTRUE);
	else if (val1 == iTRUE) push(iFALSE);
	else DIE << "Stack value expected a bool (0x00 or 0x01) but got " << HEX(val1) << "instead!";
}
void VMCore::EQ(uint32_t params[])
{
	auto val1 = pop();
	auto val2 = pop();
	if (val1 == val2) 
		push(iTRUE);
	else
		push(iFALSE);
}
void VMCore::NEQ(uint32_t params[])
{
	auto val1 = pop();
	auto val2 = pop();
	if (val1 != val2)
		push(iTRUE);
	else
		push(iFALSE);
}
void VMCore::GT(uint32_t params[])
{
	auto val1 = pop();
	auto val2 = pop();
	if (val1 > val2)
		push(iTRUE);
	else
		push(iFALSE);
}
void VMCore::GTEQ(uint32_t params[])
{
	auto val1 = pop();
	auto val2 = pop();
	if (val1 >= val2)
		push(iTRUE);
	else
		push(iFALSE);
}
void VMCore::LT(uint32_t params[])
{
	auto val1 = pop();
	auto val2 = pop();
	if (val1 < val2)
		push(iTRUE);
	else
		push(iFALSE);
}
void VMCore::LTEQ(uint32_t params[])
{
	auto val1 = pop();
	auto val2 = pop();
	if (val1 <= val2)
		push(iTRUE);
	else
		push(iFALSE);
}
void VMCore::EXIT(uint32_t params[])
{
	auto exitCode = pop();
	DIE << "Exit was called with code: " << NUM(exitCode);
	//instance.
	exit(exitCode);
}

