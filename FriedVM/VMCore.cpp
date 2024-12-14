#include "VMCore.h"
#include <thread>
#include <chrono>

void VMCore::Parse()
{
	instance.hasSymbols = binaryApi.ParseMagic();
	uint64_t instructionStart = binaryApi.ParseAddress();
	uint64_t constPoolStart = binaryApi.ParseAddress();
	uint64_t symbolsStart = 0;
	if (instance.hasSymbols)
	{
		symbolsStart = binaryApi.ParseAddress();
	}
	uint64_t totalLength = 0;
	while(instance.pc < instructionStart) //meta section
	{
		uint32_t length = binaryApi.ParseMeta();
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
	if (instance.hasSymbols && instance.pc == instructionStart) //symbols
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
				bool isUppercase = byte >= 0x41 && byte <= 0x5A;
				bool isLowercase = byte >= 0x61 && byte <= 0x7A;
				bool isUnderScore = byte == 0x5F;

				if (isUppercase || isLowercase || isUnderScore)
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

	if (instance.symbols.size() != instance.symbols_length.size())
		DIE << "symbol size does not match symbol_length size";
	if (instance.varibles.size() != instance.meta.size())
		DIE << "varible size does not match meta size";

	while(instance.pc < constPoolStart) //instructions section
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
void VMCore::syscall(uint32_t index)
{
	if (index > syscall_lookup.size())
		DIE << "Syscall at index "<< NUM(index) << " does not exist!";
	syscall_lookup.at(index)();
}
#define MAKE_EXECUTE(method) [this](uint32_t params[]) { this->method(params); }
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

	opcode_lookup[iAND].execute = MAKE_EXECUTE(AND);
	opcode_lookup[iOR].execute = MAKE_EXECUTE(OR);
	opcode_lookup[iNOT].execute = MAKE_EXECUTE(NOT);

	opcode_lookup[iEQ].execute = MAKE_EXECUTE(EQ);

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
void VMCore::SYSCALL(uint32_t params[])
{
	syscall(params[0]);
}
void VMCore::EXIT(uint32_t params[])
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
	auto idx_addr = pop();
	uint8_t *data = instance.varibles.at(idx_addr);
	uint32_t length = instance.meta.at(idx_addr);
	print_raw(data, length);
}
void VMCore::SYS_DUMP()
{
	if (!instance.hasSymbols)
		DIE << "Cant dump varible because symbols are not included";

	auto idx_addr = pop();

	uint8_t* symbol_data = instance.symbols.at(idx_addr);
	uint8_t symbol_length = instance.symbols_length.at(idx_addr);

	uint8_t* var_data = instance.varibles.at(idx_addr);
	uint32_t var_length = instance.meta.at(idx_addr);
	printf("symbol: \"");
	print_raw(symbol_data, symbol_length);
	printf("\" has value: \"");
	print_raw(var_data, var_length);
	printf("\".");
}
#pragma endregion
#pragma region Syscall_helpers
void VMCore::print_raw(uint8_t* data, uint32_t length) {
	for (uint32_t i = 0; i < length; ++i) {
		putchar(data[i]);  // Print each byte as a character
	}
}
#pragma endregion
