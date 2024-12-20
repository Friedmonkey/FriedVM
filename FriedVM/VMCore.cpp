#include "VMCore.h"
#include <thread>
#include <chrono>

void VMCore::Parse()
{
	binaryApi.ParseMagic();
	instance.instructionStart= binaryApi.ParseAddress();
	uint64_t constPoolStart = binaryApi.ParseAddress();
	uint64_t symbolsStart = 0;
	if (instance.hasSymbols)
	{
		symbolsStart = binaryApi.ParseAddress();
	}
	uint64_t totalLength = 0;
#pragma region Meta & Symbols
	while(instance.pc < instance.instructionStart) //meta section
	{
		uint64_t length = binaryApi.ParseMeta();
		auto position = totalLength + constPoolStart;
		if (instance.bytecode.size() < (position + length))
		{
			DIE << "file size was too small (" << NUM(instance.bytecode.size()) << "), expected more bytes (" << NUM(position + length) << ")";
		}
		uint8_t *buffer = new uint8_t[length];
		for (size_t i = 0; i < length; i++)
		{
			buffer[i] = instance.bytecode[position + i];
		}
		totalLength += length;

		instance.meta.push_back(length);
		instance.varibles.push_back(buffer);
	}
	if (instance.hasSymbols && instance.pc == instance.instructionStart) //symbols
	{
		totalLength = symbolsStart;
		std::vector<uint8_t> symbol_buffer;
		while (totalLength < instance.bytecode.size())
		{
			symbol_buffer.clear();
			uint8_t length = 0;
			while(totalLength+length < instance.bytecode.size())
			{
				uint8_t &byte = instance.bytecode.at(totalLength+length);
				bool isNumber = byte >= 0x30 && byte <= 0x39;
				bool isUppercase = byte >= 0x41 && byte <= 0x5A;
				bool isLowercase = byte >= 0x61 && byte <= 0x7A;
				bool isUnderScore = byte == 0x5F;

				if (isNumber || isUppercase || isLowercase || isUnderScore)
				{
					symbol_buffer.push_back(byte);
					length++;
				}
				else if (byte == symbolSplitCar)
				{
					uint8_t* buffer = new uint8_t[length];
					for (size_t i = 0; i < length; i++)
					{
						buffer[i] = instance.bytecode[totalLength + i];
					}
					totalLength += length;
					totalLength++; //split char
					instance.symbols.push_back(buffer);
					instance.symbols_length.push_back(length);
					break;
				}
				else
				{
					DIE << "The symbol at position " << NUM(totalLength) << " has inncorrect char (" << byte << ") at index " << NUM(length);
				}
			}
		}
	}
#pragma endregion
	if (instance.symbols.size() != instance.symbols_length.size())
		DIE << "symbol size does not match symbol_length size";
	if (instance.varibles.size() != instance.meta.size())
		DIE << "varible size does not match meta size";

	Run(instance.pc, constPoolStart);
}
void VMCore::Run(uint64_t start, uint64_t end)
{
	instance.pc = start;
	while (instance.pc < end) //instructions section
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
		instruction.execute(params, instruction.immediate, instruction.arg_size);
	}
}
bool VMCore::Peek_stack(uint32_t &rValue, int offset)
{
	if (instance.sp == 0)
	{
		return false;
	}
	else
	{
		rValue = instance.stack.at(instance.sp-1);
		return true;
	}
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
	instance.stack_type.pop_back();
	return value;
}
void VMCore::push(uint32_t value, bool immediate, uint8_t arg_size)
{
	instance.sp++;
	instance.stack.push_back(value);
	instance.stack_type.push_back((immediate << 7) | (arg_size & 0b01111111));
}
void VMCore::syscall(uint32_t index)
{
	if (index > syscall_lookup.size())
		DIE << "Syscall at index "<< NUM(index) << " does not exist!";
	syscall_lookup.at(index)();
}
Value VMCore::getVar()
{
	uint8_t type = instance.stack_type.at(instance.sp-1);
	bool immediate = (type & 0b10000000) >> 7;

	uint32_t value = pop();
	if (immediate)
	{
		uint32_t arg_size = static_cast<uint32_t>(type & 0b01111111);
		uint8_t *data = binaryApi.CastFromUint32(value, arg_size);
		return Value(data, arg_size, true);
	}
	else
	{
		uint8_t* data = instance.varibles.at(value);
		uint32_t &length = instance.meta.at(value);
		return Value(data, length, false, value);
	}
}
//setvar

void VMCore::Jump(uint32_t offset, bool immidiate)
{
	//if (immediate)
	//	instance.pc += params[0];
	//else
		instance.pc = instance.instructionStart + offset;
}
#define MAKE_EXECUTE(method) [this](uint32_t* params, bool immediate, uint8_t arg_size) { this->method(params, immediate, arg_size); }
#define MAKE_SYS_EXECUTE(method) [this]() { this->method(); }

VMCore::VMCore(VMInstance& newInstance) : VMInstanceBase(newInstance), binaryApi(newInstance)
{
#pragma region Instructions
	opcode_lookup[iPUSH].execute = MAKE_EXECUTE(PUSH);
	opcode_lookup[iPOP].execute = MAKE_EXECUTE(POP);
	opcode_lookup[iDUP].execute = MAKE_EXECUTE(DUP);

	opcode_lookup[iADD].execute = MAKE_EXECUTE(ADD);
	opcode_lookup[iSUB].execute = MAKE_EXECUTE(SUB);
	opcode_lookup[iMUL].execute = MAKE_EXECUTE(MUL);
	opcode_lookup[iDIV].execute = MAKE_EXECUTE(DIV);

	opcode_lookup[iINC].execute = MAKE_EXECUTE(INC);
	opcode_lookup[iDEC].execute = MAKE_EXECUTE(DEC);

	opcode_lookup[iAND].execute = MAKE_EXECUTE(AND);
	opcode_lookup[iOR].execute = MAKE_EXECUTE(OR);
	opcode_lookup[iNOT].execute = MAKE_EXECUTE(NOT);

	opcode_lookup[iEQ].execute = MAKE_EXECUTE(EQ);

	opcode_lookup[iJUMP].execute = MAKE_EXECUTE(JUMP);
	opcode_lookup[iJUMP_IF].execute = MAKE_EXECUTE(JUMP_IF);
	opcode_lookup[iJUMP_IF_STACK].execute = MAKE_EXECUTE(JUMP_IF_STACK);

	opcode_lookup[iSYSCALL].execute = MAKE_EXECUTE(SYSCALL);
	opcode_lookup[iEXIT].execute = MAKE_EXECUTE(EXIT);

#pragma endregion
#pragma region Syscalls
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_PAUSE));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_CLEAR));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_READ));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_PRINT));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_DUMP));
#pragma endregion
}

#pragma region Instructions
void VMCore::PUSH(uint32_t* params, bool immediate, uint8_t arg_size)
{
	push(params[0], immediate, arg_size);
}
void VMCore::POP(uint32_t* params, bool immediate, uint8_t arg_size)
{
	pop();
}
void VMCore::DUP(uint32_t* params, bool immediate, uint8_t arg_size)
{
	if (instance.sp == 0)
	{
		DIE << "Nothing on the stack to duplicate! At program index " << HEX(instance.pc);
	}
	auto val1 = instance.stack.at(instance.sp-1);
	push(val1);
}
void VMCore::ADD(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val2 = pop();
	auto val1 = pop();
	push(val1 + val2);
}
void VMCore::SUB(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val2 = pop();
	auto val1 = pop();
	push(val1 - val2);
}
void VMCore::MUL(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val2 = pop();
	auto val1 = pop();
	push(val1 * val2);
}
void VMCore::DIV(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val2 = pop();
	auto val1 = pop();
	push(val1 / val2);
}
void VMCore::INC(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val = pop();
	push(++val);
}
void VMCore::DEC(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val = pop();
	push(--val);
}
void VMCore::AND(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val2 = pop();
	auto val1 = pop();
	push(val1 & val2);
}
void VMCore::OR(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val1 = pop();
	auto val2 = pop();
	push(val1 | val2);
}
void VMCore::NOT(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val1 = pop();
	if (val1 == iFALSE) push(iTRUE);
	else if (val1 == iTRUE) push(iFALSE);
	else DIE << "Stack value expected a bool (0x00 or 0x01) but got " << HEX(val1) << "instead!";
}
void VMCore::EQ(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val1 = pop();
	auto val2 = pop();
	if (val1 == val2) 
		push(iTRUE);
	else
		push(iFALSE);
}
void VMCore::NEQ(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val1 = pop();
	auto val2 = pop();
	if (val1 != val2)
		push(iTRUE);
	else
		push(iFALSE);
}
void VMCore::GT(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val2 = pop();
	auto val1 = pop();
	if (val1 > val2)
		push(iTRUE);
	else
		push(iFALSE);
}
void VMCore::GTEQ(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val2 = pop();
	auto val1 = pop();
	if (val1 >= val2)
		push(iTRUE);
	else
		push(iFALSE);
}
void VMCore::LT(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val2 = pop();
	auto val1 = pop();
	if (val1 < val2)
		push(iTRUE);
	else
		push(iFALSE);
}
void VMCore::LTEQ(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val2 = pop();
	auto val1 = pop();
	if (val1 <= val2)
		push(iTRUE);
	else
		push(iFALSE);
}
void VMCore::JUMP(uint32_t* params, bool immediate, uint8_t arg_size)
{
	Jump(params[0], immediate);
}
void VMCore::JUMP_IF(uint32_t* params, bool immediate, uint8_t arg_size)
{
	uint32_t value = 0;
	if (Peek_stack(value))
	{
		if (value == iTRUE)
			Jump(params[0], immediate);
	}
}
void VMCore::JUMP_IF_STACK(uint32_t* params, bool immediate, uint8_t arg_size)
{
	uint32_t value = 0;
	if (Peek_stack(value))
	{
		if (value == params[0])
			Jump(params[1], immediate);
	}
}
void VMCore::SYSCALL(uint32_t* params, bool immediate, uint8_t arg_size)
{
	syscall(params[0]);
}
void VMCore::EXIT(uint32_t* params, bool immediate, uint8_t arg_size)
{
	uint32_t exitCode = pop();
	printf("\n\nExit was called with code: %u\n", exitCode);
	exit(exitCode);
}
#pragma endregion

#pragma region Syscalls
void VMCore::SYS_PAUSE()
{
	uint32_t pause_ms = pop();
	std::this_thread::sleep_for(std::chrono::milliseconds(pause_ms));
}
void VMCore::SYS_CLEAR()
{
	DIE << "syscall SYS_CLEAR not implemented!";
}

void VMCore::SYS_READ()
{
	DIE << "syscall SYS_READ not implemented!";
}

void VMCore::SYS_PRINT()
{
	Value val = getVar();
	print_raw(val);
}
void VMCore::SYS_DUMP()
{
	//if (!instance.hasSymbols)
	//	DIE << "Cant dump varible because symbols are not included";

	Value data = getVar();
	Value symbol = Value(data.data, std::min(data.length, (uint32_t)4)); //if we didnt use a label but imidate value, we just show the start
	if (!data.immediate && instance.hasSymbols)
	{	//if there is a label availible
		symbol.data = instance.symbols.at(data.index);
		symbol.length = instance.symbols_length.at(data.index);
	}
	printf("symbol: \"");
	print_raw(symbol);
	printf("\" has value: \"");
	print_raw(data);
	printf("\".");
}
#pragma endregion
#pragma region Syscall_helpers
void VMCore::print_raw(Value val)
{
	print_raw(val.data, val.length);
}
void VMCore::print_raw(uint8_t* data, uint32_t length) {
	for (uint32_t i = 0; i < length; ++i) {
		putchar(data[i]);  // Print each byte as a character
	}
}
#pragma endregion
