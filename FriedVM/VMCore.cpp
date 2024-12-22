#include "VMCore.h"
#include <thread>
#include <chrono>

void VMCore::Parse()
{
	binaryApi.ParseMagic();
	uint64_t emptyVarCount = binaryApi.ParseEmptyVarCount();
	instance.instructionStart= binaryApi.ParseAddress();
	uint64_t constPoolStart = binaryApi.ParseAddress();
	uint64_t symbolsStart = 0;
	if (instance.hasSymbols)
	{
		symbolsStart = binaryApi.ParseAddress();
	}
	uint64_t totalLength = 0;
#pragma region Declares & Symbols
	while(instance.pc < instance.instructionStart) //declare section
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
		instance.declare_size++;
	}
	instance.declare_size--; //size not count
	for (uint64_t i = 0; i < emptyVarCount; i++) //allocate room for empty varibles
	{
		instance.meta.push_back(0);
		instance.varibles.push_back(nullptr);
	}
	if (instance.hasSymbols && instance.pc == instance.instructionStart) //symbol section
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
uint32_t VMCore::buffer_pop()
{
	if (instance.varible_buffer.size() == 0)
		DIE << "Nothing on the buffer to pop! At program index " << HEX(instance.pc);

	uint32_t val = instance.varible_buffer.at(instance.varible_buffer.size());
	instance.varible_buffer.pop_back();
	return val;
}
void VMCore::buffer_push(Value value)
{
	instance.varible_buffer.reserve(instance.varible_buffer.size() + value.length); //reserve more space for the buffer to avoid resize
	for (uint32_t i = 0; i < value.length; i++)
	{
		instance.varible_buffer.push_back(value.data[i]);
	}
}
void VMCore::syscall(uint32_t index)
{
	if (index > syscall_lookup.size())
		DIE << "Syscall at index "<< NUM(index) << " does not exist!";
	syscall_lookup.at(index)();
}
Value VMCore::makeValue(uint32_t value, bool immediate, uint8_t arg_size)
{
	if (immediate)
	{
		uint32_t expanded_arg_size = static_cast<uint32_t>(arg_size);
		uint8_t* data = binaryApi.CastFromUint32(value, expanded_arg_size);
		return Value(data, expanded_arg_size, true);
	}
	else
	{
		checkVaribleIndex(value);
		uint32_t& length = instance.meta.at(value);
		uint8_t* data = instance.varibles.at(value);
		return Value(data, length, false, value);
	}
}
void VMCore::checkVaribleIndex(uint32_t index)
{
	if (instance.meta.size() <= index)
		DIE << "Varible with index " << HEX(index) << " does not exist!";
}
Value VMCore::getVar()
{
	if (instance.sp == 0)
	{
		DIE << "Nothing on the stack to pop! At program index " << HEX(instance.pc);
	}
	uint8_t type = instance.stack_type.at(instance.sp-1);
	bool immediate = (type & 0b10000000) >> 7;
	uint8_t arg_size = type & 0b01111111;

	uint32_t value = pop();
	return makeValue(value, immediate, arg_size);
	//if (immediate)
	//{
	//	;
	//	uint8_t *data = binaryApi.CastFromUint32(value, arg_size);
	//	return Value(data, arg_size, true);
	//}
	//else
	//{
	//	uint32_t &length = instance.meta.at(value);
	//	uint8_t* data = instance.varibles.at(value);
	//	return Value(data, length, false, value);
	//}
}
void VMCore::setVar(Value reference, Value newValue) {
	//when setting something it needs to be a ref not immidate
	if (reference.immediate) {
		DIE << "Cant assign value to immidate!";
	}

	//new value needs to have data or its kind of worthless
	if (newValue.data == nullptr) {
		DIE << "Eror trying to assign to value, cant assign nothing to value (for now)";
	}

	//its a refernce so we can use the index
	uint32_t index = reference.index;
	instance.varibles[index] = newValue.data; 
	instance.meta[index] = newValue.length;
}

void VMCore::Jump(uint32_t offset, bool immidiate)
{
	//if (immediate)
	//	instance.pc += params[0];
	//else
		instance.pc = instance.instructionStart + offset;
}
void VMCore::Call(uint32_t offset, bool immidiate)
{
	instance.call_stack.push_back(instance.pc); //keep track of where we are now
	instance.pc = instance.instructionStart + offset;
}
void VMCore::Return()
{
	if (instance.call_stack.size() == 0)
		DIE << "Call stack was empty when trying to return At program index " << HEX(instance.pc);

	instance.pc = instance.call_stack.at(instance.call_stack.size()-1);
	instance.call_stack.pop_back();
}
#define MAKE_EXECUTE(method) [this](uint32_t* params, bool immediate, uint8_t arg_size) { this->method(params, immediate, arg_size); }
#define MAKE_SYS_EXECUTE(method) [this]() { this->method(); }

VMCore::VMCore(VMInstance& newInstance) : VMInstanceBase(newInstance), binaryApi(newInstance)
{
#pragma region Instructions
	opcode_lookup[iPUSH].execute = MAKE_EXECUTE(PUSH);
	opcode_lookup[iPOP].execute = MAKE_EXECUTE(POP);
	opcode_lookup[iDUP].execute = MAKE_EXECUTE(DUP);

	opcode_lookup[iMATH].execute = MAKE_EXECUTE(MATH);

	opcode_lookup[iAND].execute = MAKE_EXECUTE(AND);
	opcode_lookup[iOR].execute = MAKE_EXECUTE(OR);
	opcode_lookup[iNOT].execute = MAKE_EXECUTE(NOT);

	opcode_lookup[iCOMP].execute = MAKE_EXECUTE(COMP);

	opcode_lookup[iJUMP].execute = MAKE_EXECUTE(JUMP);
	opcode_lookup[iJUMP_IF].execute = MAKE_EXECUTE(JUMP_IF);

	opcode_lookup[iCALL].execute = MAKE_EXECUTE(CALL);
	//opcode_lookup[iCALL_IF].execute = MAKE_EXECUTE(CALL_IF);
	opcode_lookup[iRET].execute = MAKE_EXECUTE(RET);

	opcode_lookup[iSYSCALL].execute = MAKE_EXECUTE(SYSCALL);
	opcode_lookup[iEXIT].execute = MAKE_EXECUTE(EXIT);


	opcode_lookup[iSET_BUFFER].execute = MAKE_EXECUTE(SET_BUFFER);
	opcode_lookup[iGET_BUFFER].execute = MAKE_EXECUTE(GET_BUFFER);
	opcode_lookup[iPUSH_BUFFER].execute = MAKE_EXECUTE(PUSH_BUFFER);
	opcode_lookup[iBUFFER_UTIL].execute = MAKE_EXECUTE(BUFFER_UTIL);
	opcode_lookup[iSET_VAR].execute = MAKE_EXECUTE(SET_VAR);
	//opcode_lookup[iSTRUCT_SET].execute = MAKE_EXECUTE(STRUCT_SET);
	//opcode_lookup[iSTRUCT_GET].execute = MAKE_EXECUTE(STRUCT_GET);
#pragma endregion
#pragma region Syscalls
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_PAUSE));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_CLEAR_CONSOLE));
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
	uint32_t index = instance.sp - (params[0]+1);

	if (index < 0 || params[0] + 1 > instance.sp)
	{
		DIE << "Nothing on the stack to duplicate at the index " << NUM(index) << "! At program index " << HEX(instance.pc);
	}

	uint8_t type = instance.stack_type.at(index);
	bool prevImmediate = (type & 0b10000000) >> 7;
	uint8_t prevArg_size = type & 0b01111111;

	uint32_t value = instance.stack.at(index);

	push(value, prevImmediate, prevArg_size);
}
void VMCore::MATH(uint32_t* params, bool immediate, uint8_t arg_size)
{
	uint32_t val1,val2 = 0;
	auto math_mode = params[0];
	if (math_mode != mmINC && math_mode != mmDEC)
		val2 = pop(); // Second operand
	val1 = pop(); // First operand
	uint32_t result = 0;

	switch (math_mode) {
		case mmADD: result = val1 + val2; break;
		case mmSUB: result = val1 - val2; break;
		case mmINC: result = val1 + 1; break;
		case mmDEC: result = val1 - 1; break;
		case mmMUL: result = val1 * val2; break;
		case mmDIV: result = val1 / val2; break;
		//case mmPOW: result = pow(val1, val2); break;
		//case mmROOT: result = pow(val1, 1.0 / val2); break;
		//case mmSQRT: result = sqrt(val1); break;
		default:
			DIE << "Math mode with index " << HEX(math_mode) << " does not exist!";
	}
	push(result);
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
void VMCore::COMP(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto compare_mode = params[0];
	auto val2 = pop(); // Second operand
	auto val1 = pop(); // First operand
	bool result = false;
	switch (compare_mode) {
		case cmGT: result = (val1 > val2); break;
		case cmGTE: result = (val1 >= val2); break;
		case cmLT: result = (val1 < val2); break;
		case cmLTE: result = (val1 <= val2); break;
		case cmEQ: result = (val1 == val2); break;
		case cmNEQ: result = (val1 != val2); break;
		default:
			DIE << "Compare mode with index " << HEX(compare_mode) << " does not exist!";
	}
	if (result)
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
		if (value == params[0])
			Jump(params[1], immediate);
	}
}
void VMCore::CALL(uint32_t* params, bool immediate, uint8_t arg_size)
{
	Call(params[0], immediate);
}
void VMCore::RET(uint32_t* params, bool immediate, uint8_t arg_size)
{
	Return();
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
void VMCore::SET_BUFFER(uint32_t* params, bool immediate, uint8_t arg_size)
{
	Value value = makeValue(params[0], immediate, arg_size);
	instance.varible_buffer = std::vector<uint8_t>(value.data, value.data + value.length);
}
void VMCore::GET_BUFFER(uint32_t* params, bool immediate, uint8_t arg_size)
{
	if (immediate)
		DIE << "Cant assign to immidate value!";

	uint32_t length = instance.varible_buffer.size();
	uint8_t* data = new uint8_t[length];
	std::copy(instance.varible_buffer.begin(), instance.varible_buffer.end(), data);

	uint32_t index = params[0];
	instance.varibles[index] = data;
	instance.meta[index] = length;
}
void VMCore::PUSH_BUFFER(uint32_t* params, bool immediate, uint8_t arg_size)
{
	Value value = makeValue(params[0], immediate, arg_size);
	buffer_push(value);
}

void VMCore::BUFFER_UTIL(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto buffer_mode = params[0];
	switch (buffer_mode) {
	case bmCLEAR: instance.varible_buffer.clear(); break;
	case bmPOP_TO_STACK: push(buffer_pop()); break;
	case bmPUSH_FROM_STACK: buffer_push(getVar()); break;
	//case bmREMOVE_FROM_END: 
	//{
	//}
	//case bmPOP_AND_CLEAR:
	//{
	//	instance.varible_buffer.clear();

	//}
	break;
	default:
		DIE << "Buffer mode with index " << HEX(buffer_mode) << " does not exist!";
	}
}

void VMCore::SET_VAR(uint32_t* params, bool immediate, uint8_t arg_size)
{
	Value varible = getVar();
	if (varible.immediate)
		DIE << "Cant assign to immidate value!";
	Value value = getVar();
	setVar(varible, value);
}

#pragma endregion

#pragma region Syscalls
void VMCore::SYS_PAUSE()
{
	uint32_t pause_ms = pop();
	std::this_thread::sleep_for(std::chrono::milliseconds(pause_ms));
}
void VMCore::SYS_CLEAR_CONSOLE()
{
	DIE << "syscall SYS_CLEAR not implemented!";
}

void VMCore::SYS_READ()
{
	Value buffer = getVar(); //put address on stack that we can write to
	Value val = read_raw();
	setVar(buffer, val);
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
	for (uint32_t i = 0; i < length; ++i)
		putchar(data[i]);  // Print each byte as a character
}
Value VMCore::read_raw() {
	std::vector<uint8_t> buffer;
	int value = getchar();

	while(value >= ' ' && value <= '~')
	{
		buffer.push_back(static_cast<uint8_t>(value));
		value = getchar();
	}

	//while (stdin >> value) {
	//	if (value < 0 || value > 255) break;  // Exit on out-of-range values
	//	data.push_back(static_cast<uint8_t>(value));
	//}

	uint32_t length = buffer.size();
	uint8_t* data = new uint8_t[length];
	std::copy(buffer.begin(), buffer.end(), data);
	// Now `array` points to the raw data, and `length` contains the number of elements
	return Value(data, length, true);
}
//Value VMCore::read_raw() {
//	std::vector<uint8_t> buffer;
//
//	 read bytes until enter is pressed
//	while (true) {
//		uint8_t byte = getchar();
//		if (byte == '\n' || byte == EOF)
//			break;
//		buffer.push_back(byte);
//	}
//
//	uint8_t* data = buffer.data();
//	uint32_t length = buffer.size();
//	return Value(data, length, true); //return imidate value
//}
//#pragma endregion
