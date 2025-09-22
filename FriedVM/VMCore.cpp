#include "VMCore.h"
#include <thread>
#include <random>
#include <chrono>

void VMCore::Parse()
{
	//float value = 0;
	//varible var1 = BaseValue::makeValue(vt_float_t, value);


	//std::string text = "10.5";
	//varible str = BaseValue::makeString(text);

	//BaseValue::ParseString(str, var1);

	//float output = safe_cast<float>(var1);

	/////	declare float converted;
	/////	push converted
	///// push string "10.5" //or from user input
	///// syscall parse

	//// converted is now 10.5
	//// if our input string was "bad input" or not a number
	//// then it would not convert and converted would just not be set
	//// so if u want default value then just initialize converted to whatever value u want as fallback


	//float value = 20.0;
	//varible var1 = BaseValue::makeValue(vt_float_t, value);


	//uint8_t value2 = 20;
	//varible var2 = BaseValue::makeValue(vt_uint8_t, value2);


	//varible var3 = BaseValue::EQCompareNumbers(var1, var2);
	//bool value3 = safe_cast<bool>(var3);
	//auto vlqresult = binaryApi.VLQ();


	//uint32_t value = 50;
	//varible var1 = BaseValue::makeValue(vt_uint32_t, value);
	//
	//uint32_t value2 = 20;
	//varible var2 = BaseValue::makeValue(vt_uint32_t, value2);
	//
	//varible result = BaseValue::ExecuteTyped(BaseValue::SubTypedFunctor(), var1, var2);
	////auto result = BaseValue::ExecuteTyped(BaseValue::AddTyped, var1, var2);
	//
	//uint32_t castedResult = BaseValue::getValue<uint32_t>(result);


	binaryApi.ParseMagic();
	//uint64_t emptyVarCount = binaryApi.ParseEmptyVarCount();
	instance.instructionStart = binaryApi.ParseAddress();
	instance.constPoolStart = binaryApi.ParseAddress();
	uint64_t symbolsStart = 0;
	if (instance.hasSymbols)
	{
		symbolsStart = binaryApi.ParseAddress();
	}
	uint64_t totalLength = instance.constPoolStart;
#pragma region Declares & Symbols
	instance.typed_varibles.push_back(constFalse);
	instance.typed_varibles.push_back(constTrue);

	std::vector<uint8_t> complex_buffer;
	while(instance.pc < instance.instructionStart) //declare section
	{
		varible var_type = binaryApi.ParseTypeByte(complex_buffer);
		uint64_t declaredDefault = binaryApi.VLQ();
		uint64_t declaredValue = binaryApi.VLQ();

		if (var_type->type_index == vt_bool)
		{
			for (size_t i = 0; i < declaredDefault; i++)
			{
				varible newVarible = BaseValue::makeValue(vt_bool, false);
				instance.typed_varibles.push_back(newVarible);
				newVarible->rest = instance.typed_varibles.size();
			}
			for (size_t i = 0; i < declaredValue; i++)
			{
				varible newVarible = BaseValue::makeValue(vt_bool, true);
				instance.typed_varibles.push_back(newVarible);
				newVarible->rest = instance.typed_varibles.size();
			}
			delete var_type;
			continue;
		}

		uint64_t declaredConst = 0;
		varible newDefaultVarible = nullptr;

		bool hasConst = var_type->isConst;
		var_type->isConst = false;
		if (hasConst)
			declaredConst = binaryApi.VLQ();

		bool useDefault = (hasConst || (declaredDefault > 0));
		if (useDefault)
		{
			newDefaultVarible = BaseValue::dupValue(var_type);
			BaseValue::FillDefaultValue(newDefaultVarible);
		}


		if (declaredDefault > 0)
		{
			for (size_t i = 0; i < declaredDefault; i++)
			{
				varible newVarible = BaseValue::dupValue(newDefaultVarible);
				instance.typed_varibles.push_back(newVarible);
				newVarible->rest = instance.typed_varibles.size();
			}
		}
		for (size_t i = 0; i < declaredValue; i++)
		{
			varible newVarible = BaseValue::dupValue(var_type);
			binaryApi.FillData(&totalLength, newVarible);
			/*size_t size = BaseValue::getTypeSize(newVarible->type_index);
			totalLength += size;

			auto length = newVarible->length + size;
			uint8_t *buffer = new uint8_t[length];
			for (size_t i = 0; i < length; i++)
			{
				if (i > newVarible->length)
				{
					buffer[i] = instance.bytecode[position + i];
				}
				else
				{
					buffer[i] = newVarible->data[i];
				}
			}

			delete[] newVarible->data;
			newVarible->data = buffer;*/
			instance.typed_varibles.push_back(newVarible);
			newVarible->rest = instance.typed_varibles.size();
		}
		if (hasConst)
		{
			varible newVarible = BaseValue::dupValue(newDefaultVarible);
			newVarible->isConst = true;
			instance.typed_varibles.push_back(newVarible);
			newVarible->rest = instance.typed_varibles.size();

			for (size_t i = 0; i < declaredConst; i++)
			{
				varible newVarible = BaseValue::dupValue(var_type);
				newVarible->isConst = true;
				binaryApi.FillData(&totalLength, newVarible);
				instance.typed_varibles.push_back(newVarible);
				newVarible->rest = instance.typed_varibles.size();
			}
		}

		if (useDefault)
		{
			delete newDefaultVarible;
		}
		delete var_type;
		////uint8_t type_byte = 

		//if (instance.bytecode.size() < (position + length))
		//{
		//	DIE << "file size was too small (" << NUM(instance.bytecode.size()) << "), expected more bytes (" << NUM(position + length) << ")";
		//}
		//uint8_t *buffer = new uint8_t[length];
		//for (size_t i = 0; i < length; i++)
		//{
		//	buffer[i] = instance.bytecode[position + i];
		//}

		//instance.meta.push_back(length);
		//instance.varibles.push_back(buffer);
		//instance.declare_size++;
	}
	instance.declare_size--; //size not count
	//for (uint64_t i = 0; i < emptyVarCount; i++) //allocate room for empty varibles
	//{
	//	instance.meta.push_back(0);
	//	instance.varibles.push_back(nullptr);
	//}
	if (instance.hasSymbols && instance.pc == instance.instructionStart) //symbol section
	{
		std::string constFalseSymbol = "false";
		uint8_t* falseBuffer = new uint8_t[constFalseSymbol.size()];
		std::memcpy(falseBuffer, constFalseSymbol.data(), constFalseSymbol.size());
		instance.symbols.push_back(falseBuffer);
		instance.symbols_length.push_back(constFalseSymbol.size());

		std::string constTrueSymbol = "true";
		uint8_t* trueBuffer = new uint8_t[constTrueSymbol.size()];
		std::memcpy(trueBuffer, constTrueSymbol.data(), constTrueSymbol.size());
		instance.symbols.push_back(trueBuffer);
		instance.symbols_length.push_back(constTrueSymbol.size());

		totalLength = symbolsStart;
		std::vector<uint8_t> symbol_buffer;
		uint32_t skipped = 0;
		while (totalLength < instance.bytecode.size())
		{
			symbol_buffer.clear();

			while (totalLength < instance.bytecode.size())
			{
				uint8_t byte = instance.bytecode.at(totalLength);
				bool isNumber = byte >= 0x30 && byte <= 0x39;
				bool isUppercase = byte >= 0x41 && byte <= 0x5A;
				bool isLowercase = byte >= 0x61 && byte <= 0x7A;
				bool isUnderScore = byte == 0x5F;

				// handle special case for underscore at start
				if (symbol_buffer.empty() && isUnderScore && instance.compact)
				{
					totalLength++; //skip the underscore
					uint64_t amount = binaryApi.offsetted_VLQ(&totalLength);
					for (size_t i = 0; i < amount; i++)
					{
						std::string str = "_" + std::to_string(skipped);
						uint8_t* buffer = new uint8_t[str.size()];
						std::memcpy(buffer, str.data(), str.size());

						instance.symbols.push_back(buffer);
						instance.symbols_length.push_back(str.size());
						skipped++;
					}
					if (instance.bytecode.at(totalLength) != symbolSplitCar)
						DIE << "Expected an ending character after a compact symbol at program index " << HEX(totalLength);
					
					totalLength++; //skip splitChar
					continue;
				}

				if (isNumber || isUppercase || isLowercase || isUnderScore)
				{
					symbol_buffer.push_back(byte);
					totalLength++;
				}
				else if (byte == symbolSplitCar)
				{
					totalLength++; // consume the split char

					uint8_t* buffer = new uint8_t[symbol_buffer.size()];
					std::memcpy(buffer, symbol_buffer.data(), symbol_buffer.size());

					instance.symbols.push_back(buffer);
					instance.symbols_length.push_back(symbol_buffer.size());

					break; // done with this symbol
				}
				else
				{
					DIE << "The symbol at position " << NUM(totalLength)
						<< " has incorrect char (" << byte << ") at symbol index "
						<< NUM(symbol_buffer.size());
				}
			}
		}

	}
#pragma endregion
	if (instance.symbols.size() != instance.symbols_length.size())
		DIE << "symbol size does not match symbol_length size";
	if (instance.varibles.size() != instance.meta.size())
		DIE << "varible size does not match meta size";

	Run(instance.pc, instance.constPoolStart);
}
void VMCore::Run(uint64_t start, uint64_t end)
{
	std::vector<Statement> Statements;
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
		ErrorLogMessage::current_instruction = instruction.op_name;

		auto params = binaryApi.GetParams(instruction);
		
		Statements.push_back(Statement(instruction, params));
	}


	while (instance.ProgramIndex < Statements.size())
	{
		auto& statement = Statements[instance.ProgramIndex];

		bool advanceProgramIndex = statement.Instruction.execute(statement.params);

		if (advanceProgramIndex)
			instance.ProgramIndex++;
	}


	instance.ProgramIndex = -1;
	while (instance.ProgramIndex < Statements.size())
	{
		instance.ProgramIndex++;
		//do bounds check or something
		auto& statement = Statements[instance.ProgramIndex];

		statement.Instruction.execute(statement.params);
	}
}

static uint64_t GetVarVLQ(varible var, uint64_t *offset)
{
	uint64_t value = 0;
	uint8_t shift = 0;
	uint8_t byte = 0;

	do
	{
		if (var->length < (*offset + 1))
		{
			DIE << "file size was too small (" << NUM(var->length) << "), expected more bytes (" << NUM(*offset + 1) << ")";
		}
		byte = var->data[*offset++];
		value |= (uint64_t)(byte & 0x7F) << shift; // Mask out MSB and shift
		shift += 7;

		if (shift >= 64) // Prevent overflow
		{
			DIE << "VLQ decoding error: shift exceeded 64 bits, possibly malformed data.";
		}

	} while (byte & 0x80); // Continue if MSB is 1

	return value;
}
bool VMCore::typed_Peek_stack(varible *output, int offset)
{
	if (instance.sp == 0)
	{
		return false;
	}
	else
	{
		*output = instance.typed_stack.at(instance.sp - 1);
		return true;
	}
}
bool VMCore::Peek_stack(uint32_t *pValue, int offset)
{
	if (instance.sp == 0)
	{
		return false;
	}
	else
	{
		*pValue = instance.stack.at(instance.sp-1);
		return true;
	}
}
bool VMCore::typed_StackTruthy()
{
	varible stack_value;
	if (!typed_Peek_stack(&stack_value))
		return false;
	return stack_value->isTruthy();
}
bool VMCore::typed_StackEquals(varible param_value)
{
	varible stack_value;
	if (!typed_Peek_stack(&stack_value))
		return false;

	return BaseValue::Equals(stack_value, param_value);
}
bool VMCore::StackEquals(Value &param_value)
{
	bool success = false;
	uint32_t stck_value = 0;

	bool stck_immediate = false;
	uint8_t stck_arg_size = 0;
	success |= Peek_stack(&stck_value);
	success |= getStackType(&stck_immediate, &stck_arg_size);
	if (success)
	{
		Value stack_value = makeValue(stck_value, stck_immediate, stck_arg_size);
		if (stack_value.length != param_value.length)
			return false;

		for (uint32_t i = 0; i < stack_value.length; i++)
		{
			if (stack_value.data[i] != param_value.data[i])
				return false;
		}
		return true;
	}
	return false;
}

template<typename T>
T VMCore::safe_cast(varible var)
{
	return BaseValue::getValue<T>(var);
}
varible VMCore::typed_pop()
{
	if (instance.sp == 0)
	{
		DIE << "Nothing on the stack to pop! At program index " << NUM(instance.ProgramIndex);
	}
	instance.sp--;
	varible value = instance.typed_stack.at(instance.sp);
	instance.typed_stack.pop_back();
	return value;
}
void VMCore::typed_push(varible value)
{
	instance.sp++;
	instance.typed_stack.push_back(value);
}
varible VMCore::dup(varible value)
{
	return BaseValue::dupValue(value);
}

void VMCore::freeVarible(uint32_t index)
{
	checkVaribleIndex(index);

	instance.meta[index] = 0;
	if (instance.varibles[index] != nullptr) {
		delete[] instance.varibles[index];  // Free the previously allocated memory
		instance.varibles[index] = nullptr;
	}
}
uint32_t VMCore::pop()
{
	if (instance.sp == 0)
	{
		DIE << "Nothing on the stack to pop! At program index " << NUM(instance.ProgramIndex);
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
		DIE << "Nothing on the buffer to pop! At program index " << NUM(instance.ProgramIndex);

	uint32_t val = instance.varible_buffer.at(instance.varible_buffer.size()-1);
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
		DIE << "Varible with index " << NUM(index) << "(" << HEX(index) << ") does not exist!";
}

void VMCore::GetStructCache(Value &struct_instance, StructCache *cache)
{
	if (struct_instance.length < structIndexByteCount)
		DIE << "Invalid struct passed (too small for index)";

	uint32_t index = binaryApi.CastToUint32(struct_instance.data, structIndexByteCount);
	if (instance.structCache.find(index) != instance.structCache.end())
		*cache = instance.structCache[index];
	else
		*cache = CacheStructDefinition(index);
}
void VMCore::GetStructFieldDetails(Value &struct_instance, uint32_t field_index, uint32_t *field_offset, uint8_t *field_length)
{
	StructCache cache;
	GetStructCache(struct_instance, &cache);

	if (struct_instance.length < cache.total_size)
		DIE << "Struct instance was too small";

	if (field_index >= cache.offsets.size())
		DIE << "Field index out of bounds";

	*field_offset = cache.offsets[field_index];
	*field_length = cache.lengths[field_index];
	return;
}
StructCache VMCore::CacheStructDefinition(uint32_t index)
{
	Value struct_definition = makeValue(index, false, 0); //use the first few byte/index to get the declare/definition
	const uint8_t struct_offset = 2;

	if (struct_definition.length < struct_offset || struct_definition.data[0] != 0xFF)
		DIE << "Invalid struct definition";

	uint8_t amount_of_fields = struct_definition.data[1];

	if (struct_definition.length < (amount_of_fields + struct_offset))
		DIE << "Struct instance is too small";

	StructCache cache;
	size_t sum = 0;
	for (uint8_t i = 0; i < amount_of_fields; i++)
	{
		uint8_t length = struct_definition.data[struct_offset + i];
		cache.lengths.push_back(length);
		cache.offsets.push_back(sum + structIndexByteCount);
		sum += length;
	}

	if (struct_definition.length < sum)
		DIE << "Invalid struct passed, size was too small!";

	cache.total_size = sum + structIndexByteCount;
	cache.initial_data = new uint8_t[cache.total_size];

	//copy the struct definition address/index as 4 bytes (structIndexByteCount = 4 bytes)
	uint8_t* index_bytes = binaryApi.CastFromUint32(index, structIndexByteCount);
	std::copy(index_bytes, index_bytes + structIndexByteCount, cache.initial_data);

	//copy the initial data, wich is at the end of the definition from data_start_index onwards
	size_t data_start_index = struct_offset + amount_of_fields;
	std::copy(struct_definition.data + data_start_index, struct_definition.data + struct_definition.length, cache.initial_data + structIndexByteCount);

	instance.structCache[index] = cache;
	return cache;
}
bool VMCore::getStackType(bool *immediate, uint8_t *arg_size)
{
	if (instance.sp == 0)
	{
		return false;
	}
	else
	{
		uint8_t type = instance.stack_type.at(instance.sp - 1);
		bool stck_immediate = (type & 0b10000000) >> 7;
		uint8_t stck_arg_size = type & 0b01111111;

		*immediate = stck_immediate;
		*arg_size = stck_arg_size;
		return true;
	}
}
uint32_t VMCore::makeUint(uint32_t value, bool immediate, uint8_t arg_size)
{
	if (immediate)
	{
		return value;
	}
	else
	{
		Value val = makeValue(value, immediate, arg_size);
		if (val.length > 4)
			DIE << "Expected a number wich is max 4 bytes, but got " << NUM(val.length) << " bytes instead!";

		uint32_t result = binaryApi.CastToUint32(val.data, val.length);
		return result;
	}
}
uint32_t VMCore::getUintVar()
{
	bool immediate = false;
	uint8_t arg_size = 0;
	bool success = getStackType(&immediate, &arg_size);
	if (!success)
	{
		DIE << "Nothing on the stack to pop! At program index " << NUM(instance.ProgramIndex);
	}

	uint32_t value = pop();

	return makeUint(value, immediate, arg_size);
}
Value VMCore::getVar()
{
	bool immediate = false;
	uint8_t arg_size = 0;
	bool success = getStackType(&immediate, &arg_size);
	if (!success)
	{
		DIE << "Nothing on the stack to pop! At program index " << NUM(instance.ProgramIndex);
	}

	uint32_t value = pop();
	return makeValue(value, immediate, arg_size);
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
	freeVarible(index);
	instance.varibles[index] = newValue.data; 
	instance.meta[index] = newValue.length;
}
void VMCore::typed_setVar(varible reference, varible newValue) {
	//when setting something it needs to be a ref not immidate
	//if (reference.immediate) {
	//	DIE << "Cant assign value to immidate!";
	//}

	//new value needs to have data or its kind of worthless
	if (newValue->data == nullptr) {
		DIE << "Eror trying to assign to value, cant assign nothing to value (for now)";
		return;
	}

	if (reference->isNumber() && newValue->isNumber()) {
		// cast newValue into reference type safely
		varible casted = BaseValue::zero_cast(newValue, reference->type_index);

		// overwrite existing data
		std::memcpy(reference->data, casted->data, reference->length);
		//delete casted; // cleanup temporary
		return;
	}

	if (!(reference->type_index == newValue->type_index))
		DIE << "incompatible types! trying to assign " << newValue->type_index << " to a " << reference->type_index;


	if (reference->length != newValue->length) //same type but length does not match?
	{	//we reallocate space of the correct size i guess
		delete[] reference->data;
		reference->data = new uint8_t[newValue->length];
		reference->length = newValue->length;
	}

	std::memcpy(reference->data, newValue->data, newValue->length);

	//BaseValue::computeNumbers(val1, val2, BaseValue::AddOperation{})

	////if (newValue->type_index != reference->type_index)
	////{
	////	DIE << "cant assing diffrent types or smth idk?";
	////	//this is pretty string can assing string to raw
	////	//cant assing uint8 to uint32
	////	//need better typcompatability system
	////	//either that or just overwrite it lol
	////}
	//reference->type_index = newValue->type_index;


	////delete[]
	////free(reference->data);


	//reference->length = newValue->length;
	////std::memcpy(reference->data, newValue->data, reference->length);
	//reference->data = newValue->data;


	////its a refernce so we can use the index
	////uint32_t index = reference.index;
	////freeVarible(index);
	////instance.varibles[index] = newValue.data;
	////instance.meta[index] = newValue.length;
}

void VMCore::Jump(varible offset)
{
	if (offset->type_index != vt_label)
		DIE << "Jump expected a label but got type " << HEX(offset->type_index) << " instead";

	uint64_t idx = safe_cast<uint64_t>(offset);
	instance.ProgramIndex = idx;
}
void VMCore::Jump(uint32_t offset, bool immidiate)
{
	//if (immediate)
	//	instance.pc += params[0];
	//else
		instance.ProgramIndex = offset;// instance.instructionStart + offset;
}
void VMCore::Call(varible offset)
{
	if (offset->type_index != vt_label)
		DIE << "Jump expected a label but got type " << HEX(offset->type_index) << " instead";

	uint64_t idx = safe_cast<uint64_t>(offset);
	instance.call_stack.push_back(instance.ProgramIndex); //keep track of where we are now
	instance.ProgramIndex = idx;
}
void VMCore::Return()
{
	if (instance.call_stack.size() == 0)
		DIE << "Call stack was empty when trying to return At program index " << NUM(instance.ProgramIndex);

	instance.ProgramIndex = instance.call_stack.at(instance.call_stack.size()-1);
	instance.call_stack.pop_back();
}
//#define MAKE_EXECUTE(method) [this](uint32_t* params, bool immediate, uint8_t arg_size) { this->method(params, immediate, arg_size); }
#define MAKE_EXECUTE(method) [this](varible* params) { return this->method(params); }
#define MAKE_SYS_EXECUTE(method) [this]() { this->method(); }

VMCore::VMCore(VMInstance& newInstance) : VMInstanceBase(newInstance), binaryApi(newInstance)
{
	constFalse = BaseValue::makeValue(vt_bool, false);
	constFalse->isConst = true;

	constTrue = BaseValue::makeValue(vt_bool, true);
	constTrue->isConst = true;

#pragma region Instructions
	opcode_lookup[iEXIT].execute = MAKE_EXECUTE(typed_EXIT);
	opcode_lookup[iSYSCALL].execute = MAKE_EXECUTE(typed_SYSCALL);

	opcode_lookup[iPUSH].execute = MAKE_EXECUTE(typed_PUSH);
	opcode_lookup[iSTORE].execute = MAKE_EXECUTE(typed_STORE);
	opcode_lookup[iSET_VAR].execute = MAKE_EXECUTE(typed_SET_VAR);
	opcode_lookup[iPOP].execute = MAKE_EXECUTE(typed_POP);
	opcode_lookup[iSWAP].execute = MAKE_EXECUTE(typed_SWAP);
	opcode_lookup[iDUP].execute = MAKE_EXECUTE(typed_DUP);

	opcode_lookup[iMATH].execute = MAKE_EXECUTE(typed_MATH);
	opcode_lookup[iINC].execute = MAKE_EXECUTE(typed_INC);
	opcode_lookup[iDEC].execute = MAKE_EXECUTE(typed_DEC);

	opcode_lookup[iCOMP].execute = MAKE_EXECUTE(typed_COMP);
	opcode_lookup[iCHECK_STACK].execute = MAKE_EXECUTE(typed_CHECK_STACK);
	opcode_lookup[iNOT].execute = MAKE_EXECUTE(typed_NOT);

	opcode_lookup[iJUMP].execute = MAKE_EXECUTE(typed_JUMP);
	opcode_lookup[iJUMP_IF].execute = MAKE_EXECUTE(typed_JUMP_IF);
	opcode_lookup[iJUMP_IF_STACK].execute = MAKE_EXECUTE(typed_JUMP_IF_STACK);

	opcode_lookup[iCALL].execute = MAKE_EXECUTE(typed_CALL);
	opcode_lookup[iCALL_IF].execute = MAKE_EXECUTE(typed_CALL_IF);
	opcode_lookup[iCALL_IF_STACK].execute = MAKE_EXECUTE(typed_CALL_IF_STACK);

	opcode_lookup[iRET].execute = MAKE_EXECUTE(typed_RET);




	//opcode_lookup[iSET_BUFFER].execute = MAKE_EXECUTE(SET_BUFFER);
	//opcode_lookup[iPUSH_BUFFER].execute = MAKE_EXECUTE(PUSH_BUFFER);
	//opcode_lookup[iGET_BUFFER].execute = MAKE_EXECUTE(GET_BUFFER);
	//opcode_lookup[iBUFFER_UTIL].execute = MAKE_EXECUTE(BUFFER_UTIL);
	//opcode_lookup[iSET_STRUCT].execute = MAKE_EXECUTE(SET_STRUCT);
	//opcode_lookup[iGET_STRUCT].execute = MAKE_EXECUTE(GET_STRUCT);
	//opcode_lookup[iCREATE_STRUCT].execute = MAKE_EXECUTE(CREATE_STRUCT);

#pragma endregion
#pragma region Syscalls
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_PAUSE));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_CLEAR_CONSOLE));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_READ));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_PRINT));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_PRINTLN));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_DUMP));

	//syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_TO_STRING_UNSIGNED));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_TO_STRING_SIGNED));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_PARSE));
	//syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_TO_NUMBER_UNSIGNED));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_TO_NUMBER_SIGNED));

	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_INPUT_MODE_READ));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_INPUT_MODE_WRITE));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_INPUT_TO_STRUCT));

	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_SET_CONSOLE_CURSOR));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_GET_CONSOLE_CURSOR));
#pragma endregion
}

#pragma region typed_Instructions
bool VMCore::typed_PUSH(varible* params)
{
	typed_push(params[0]);
	return true;
}
bool VMCore::typed_STORE(varible* params)
{
	varible var = params[0];
	if (var->isConst)
		DIE << "Cant assign to const value!";
	varible value = typed_pop();
	typed_setVar(var, value);
	return true;
}
bool VMCore::typed_SET_VAR(varible* params)
{
	varible var = typed_pop();
	if (var->isConst)
		DIE << "Cant assign to const value!";
	varible value = typed_pop();
	typed_setVar(var, value);
	return true;
}
bool VMCore::typed_POP(varible* params)
{
	typed_pop();
	return true;
}
bool VMCore::typed_SWAP(varible* params)
{
	if (instance.sp < 2)
	{
		DIE << "Nothing on the stack to swap we need 2 values! At program index " << NUM(instance.ProgramIndex);
	}

	varible topMost = typed_pop();
	varible secondTopMost = typed_pop();

	typed_push(topMost);
	typed_push(secondTopMost);
	return true;
}
bool VMCore::typed_DUP(varible* params)
{
	uint32_t index = instance.sp - 1;

	if (index < 0 || 1 > instance.sp)
	{
		DIE << "Nothing on the stack to duplicate at the index " << NUM(index) << "! At program index " << NUM(instance.ProgramIndex);
	}

	varible value = dup(instance.typed_stack.at(index));
	typed_push(value);
	return true;
}
bool VMCore::typed_MATH(varible* params)
{
	varible val1 = 0, val2 = 0;

	uint32_t math_mode = safe_cast<uint32_t>(params[0]);
	//uint32_t math_mode = makeUint(params[0], immediate, arg_size);

	// Initialize random engine with a device


	if (math_mode != mmINC && math_mode != mmDEC)
		val2 = typed_pop();
	val1 = typed_pop();
	//val1 = getUintVar(); // First operand
	varible result = 0;
	varible one = BaseValue::makeValue(vt_uint32_t, 1);

	switch (math_mode) {
	case mmADD: result = BaseValue::computeNumbers(val1, val2, BaseValue::AddOperation{}); break;
	case mmSUB: result = BaseValue::computeNumbers(val1, val2, BaseValue::SubOperation{}); break;
	case mmINC: result = BaseValue::computeNumbers(val1, one, BaseValue::AddOperation{}); break;
	case mmDEC: result = BaseValue::computeNumbers(val1, one, BaseValue::SubOperation{}); break;
	case mmMUL: result = BaseValue::computeNumbers(val1, val2, BaseValue::MulOperation{}); break;
	case mmDIV: result = BaseValue::computeNumbers(val1, val2, BaseValue::DivOperation{}); break;
		//case mmPOW: result = pow(val1, val2); break;
		//case mmROOT: result = pow(val1, 1.0 / val2); break;
		//case mmSQRT: result = sqrt(val1); break;
	case mmRAND: result = BaseValue::computeNumbers(val1, val2, BaseValue::RNDOperation{}); break;
	default:
		DIE << "Math mode with index " << HEX(math_mode) << " does not exist!";
	}
	typed_push(result);
	return true;
}
bool VMCore::typed_INC(varible* params)
{
	varible var = params[0];
	if (var->isConst)
		DIE << "cannot assign to const";

	varible one = BaseValue::makeValue(vt_uint32_t, 1);

	varible result = BaseValue::computeNumbers(var, one, BaseValue::AddOperation{});
	typed_setVar(var, result);
	return true;
}
bool VMCore::typed_DEC(varible* params)
{
	varible var = params[0];
	if (var->isConst)
		DIE << "cannot assign to const";

	varible one = BaseValue::makeValue(vt_uint32_t, 1);

	varible result = BaseValue::computeNumbers(var, one, BaseValue::SubOperation{});
	typed_setVar(var, result);
	return true;
}
//bool VMCore::typed_AND(varible* params)
//{
//	varible val2 = typed_pop();
//	varible val1 = typed_pop();
//
//	varible result = And(val1, val2);
//	typed_push(result);
//	return true;
//}
//
//bool VMCore::typed_OR(varible* params)
//{
//	varible val2 = typed_pop();
//	varible val1 = typed_pop();
//
//	varible result = Or(val1, val2);
//	typed_push(result);
//	return true;
//}



bool VMCore::typed_COMP(varible* params)
{
	//varible bTrue = BaseValue::makeValue(vt_bool, (uint8_t)1);
	//varible bFalse = BaseValue::makeValue(vt_bool, (uint8_t)0);

	uint32_t compare_mode = safe_cast<uint32_t>(params[0]);
	varible val2 = typed_pop();
	varible val1 = typed_pop(); 

	varible result;
	switch (compare_mode) {
	case cmEQ: result = BaseValue::computeNumbers(val1,val2, BaseValue::EQCompOperation{}); break;
	case cmNEQ: result = BaseValue::computeNumbers(val1, val2, BaseValue::NEQCompOperation{}); break;
	case cmGT: result = BaseValue::computeNumbers(val1, val2, BaseValue::GTCompOperation{}); break;
	case cmGTE: result = BaseValue::computeNumbers(val1, val2, BaseValue::GTECompOperation{}); break;
	case cmLT: result = BaseValue::computeNumbers(val1, val2, BaseValue::LTCompOperation{}); break;
	case cmLTE: result = BaseValue::computeNumbers(val1, val2, BaseValue::LTECompOperation{}); break;
	default:
		DIE << "Compare mode with index " << HEX(compare_mode) << " does not exist!";
	}
	if (result->isTruthy())
		typed_push(constTrue);
	else
		typed_push(constFalse);
	return true;
}

bool VMCore::typed_CHECK_STACK(varible* params)
{
	bool stackEQ = typed_StackEquals(params[0]);
	if (stackEQ)
		typed_push(constTrue);
	else
		typed_push(constFalse);

	return true;
}
bool VMCore::typed_NOT(varible* params)
{
	varible val1 = typed_pop();
	if (val1->isBoolTrue())
		typed_push(constFalse);
	else
		typed_push(constTrue);

	return true;
}

bool VMCore::typed_JUMP(varible* params)
{
	Jump(params[0]);
	return false; //dont advance the program index
}

bool VMCore::typed_JUMP_IF(varible* params)
{
	varible stackValue = typed_pop();
	if(stackValue->isTruthy())
	{
		Jump(params[0]);
		return false; //dont advance the program index
	}
	return true; //advance program index
}

bool VMCore::typed_JUMP_IF_STACK(varible* params)
{
	varible stackValue = typed_pop();
	if (BaseValue::Equals(stackValue, params[0]))
	{
		Jump(params[1]);
		return false; //dont advance the program index
	}
	return true; //advance program index
}

bool VMCore::typed_CALL(varible* params)
{
	Call(params[0]);
	return false; //dont advance the program index
}

bool VMCore::typed_CALL_IF(varible* params)
{
	varible stackValue = typed_pop();
	if (stackValue->isTruthy())
	{
		Call(params[0]);
		return false; //dont advance the program index
	}
	return true; //advance program index
}

bool VMCore::typed_CALL_IF_STACK(varible* params)
{
	varible stackValue = typed_pop();
	if (BaseValue::Equals(stackValue, params[0]))
	{
		Call(params[1]);
		return false; //dont advance the program index
	}
	return true; //advance program index
}

bool VMCore::typed_RET(varible* params)
{
	Return();
	return true; //advance the program index otherwise we end up running the call again or whatever!
}

bool VMCore::typed_SYSCALL(varible* params)
{
	uint32_t idx = safe_cast<uint32_t>(params[0]);
	syscall(idx);
	return true;
}

bool VMCore::typed_EXIT(varible* params)
{
	inputManager.setInputModePrinting(); //reset cursor mode
	int32_t exitCode = safe_cast<int32_t>(params[0]);
	printf("\n\nExit was called with code: %d\n", exitCode);
	exit(exitCode);
	return true;
}
#pragma endregion
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
		DIE << "Nothing on the stack to duplicate at the index " << NUM(index) << "! At program index " << NUM(instance.ProgramIndex);
	}

	uint8_t type = instance.stack_type.at(index);
	bool prevImmediate = (type & 0b10000000) >> 7;
	uint8_t prevArg_size = type & 0b01111111;

	uint32_t value = instance.stack.at(index);

	push(value, prevImmediate, prevArg_size);
}
void VMCore::MATH(uint32_t* params, bool immediate, uint8_t arg_size)
{
	uint32_t val1 = 0,val2 = 0;
	
	uint32_t math_mode = makeUint(params[0], immediate, arg_size);

	// Initialize random engine with a device
	static std::random_device rd;
	static std::mt19937 gen(rd());  // Mersenne Twister engine for randomness

	if (math_mode != mmINC && math_mode != mmDEC)
		val2 = getUintVar(); // Second operand
	val1 = getUintVar(); // First operand
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
		case mmRAND:
		{
			std::uniform_int_distribution<> distrib(val1, val2);

			// Generate a random number between min and max
			result = distrib(gen);
			break;
		}
		default:
			DIE << "Math mode with index " << HEX(math_mode) << " does not exist!";
	}
	push(result);
}
void VMCore::AND(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val2 = getUintVar();
	auto val1 = getUintVar();
	push(val1 & val2);
}
void VMCore::OR(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val1 = getUintVar();
	auto val2 = getUintVar();
	push(val1 | val2);
}
void VMCore::NOT(uint32_t* params, bool immediate, uint8_t arg_size)
{
	auto val1 = getUintVar();
	if (val1 == iFALSE) push(iTRUE);
	else if (val1 == iTRUE) push(iFALSE);
	else DIE << "Stack value expected a bool (0x00 or 0x01) but got " << HEX(val1) << "instead!";
}
void VMCore::COMP(uint32_t* params, bool immediate, uint8_t arg_size)
{
	uint32_t compare_mode = makeUint(params[0], immediate, arg_size);
	auto val2 = getUintVar(); // Second operand
	auto val1 = getUintVar(); // First operand
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
	if (getUintVar() == iTRUE)
	{
		Jump(params[0], immediate);
	}
}
void VMCore::CALL(uint32_t* params, bool immediate, uint8_t arg_size)
{
	//Call(params[0], immediate);
}
void VMCore::CALL_IF(uint32_t* params, bool immediate, uint8_t arg_size)
{
	if (getUintVar() == iTRUE)
	{
		//Call(params[0], immediate);
	}
}
void VMCore::RET(uint32_t* params, bool immediate, uint8_t arg_size)
{
	Return();
}
void VMCore::SYSCALL(uint32_t* params, bool immediate, uint8_t arg_size)
{
	syscall(makeUint(params[0], immediate, arg_size));
}
void VMCore::EXIT(uint32_t* params, bool immediate, uint8_t arg_size)
{
	inputManager.setInputModePrinting(); //reset cursor mode
	uint32_t exitCode = getUintVar();
	printf("\n\nExit was called with code: %d\n", exitCode);
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
	uint32_t buffer_mode = makeUint(params[0], immediate, arg_size);
	switch (buffer_mode) {
	case bmCLEAR: instance.varible_buffer.clear(); break;
	case bmPOP_TO_STACK: push(buffer_pop()); break;
	case bmPUSH_FROM_STACK: buffer_push(getVar()); break;
	//case bmREMOVE_FROM_END: 
	//{
	//}
	case bmFORMAT:
	{
		std::vector<uint8_t> &buffer = instance.varible_buffer;

		std::vector<uint8_t> formatted_buffer;
		formatted_buffer.reserve(buffer.size()); //atleast this small

		size_t index = 0;
		while(index < buffer.size())
		{
			size_t specifier_index = index;
			//skip normal text
			while(specifier_index < buffer.size() && instance.varible_buffer[specifier_index] != '%')
				specifier_index++;

			
			if (specifier_index > index) //whats the point of copying noting
				std::copy(buffer.begin()+index, buffer.begin()+specifier_index, std::back_inserter(formatted_buffer));

			if (specifier_index >= buffer.size())
				break; //no more format specifiers left, we're done

			if (specifier_index+1 >= buffer.size())
				DIE << "Incorrect format, '%' at the end!";

			//we found % + a char for format, get value from stack and format it
			char specifier = buffer[specifier_index+1];
			Value stack_val = getVar();

			std::string formatted;
			switch (specifier)
			{
				case 'u': //unsigned int
				{
					uint32_t unsigned_val = binaryApi.CastToUint32(stack_val.data, stack_val.length);
					formatted = std::to_string(unsigned_val);
					break;
				}
				case 'd': //signed int
				{
					uint32_t unsigned_val = binaryApi.CastToUint32(stack_val.data, stack_val.length);
					int32_t val = static_cast<int32_t>(unsigned_val);
					formatted = std::to_string(val);
					break;
				}
				case 's': //string
				{
					formatted.assign(stack_val.data, stack_val.data + stack_val.length);
					break;
				}
				default:
					DIE << "Unknown format specifier: %" << specifier;
					break;
			}

			//append the formatted string
			std::copy(formatted.begin(), formatted.end(), std::back_inserter(formatted_buffer));

			//skip over % and format specifier like "%s"
			index = specifier_index + 2;
		}

		instance.varible_buffer = std::move(formatted_buffer);
		break;
	}
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
void VMCore::SET_STRUCT(uint32_t* params, bool immediate, uint8_t arg_size)
{
	Value struct_instance = makeValue(params[0], false, arg_size);
	uint32_t field_index = params[1];

	uint32_t field_offset = 0;
	uint8_t field_length = 0;

	GetStructFieldDetails(struct_instance, field_index, &field_offset, &field_length);

	uint32_t value = getUintVar();
	uint8_t *data = binaryApi.CastFromUint32(value, field_length);
	std::copy(data, data+field_length, struct_instance.data+field_offset);
}

void VMCore::GET_STRUCT(uint32_t* params, bool immediate, uint8_t arg_size)
{
	Value struct_instance = makeValue(params[0], false, arg_size);
	uint32_t field_index = params[1];

	uint32_t field_offset = 0;
	uint8_t field_length = 0;

	GetStructFieldDetails(struct_instance, field_index, &field_offset, &field_length);

	uint32_t data = binaryApi.CastToUint32(&struct_instance.data[field_offset], field_length);
	push(data);
}
void VMCore::CREATE_STRUCT(uint32_t* params, bool immediate, uint8_t arg_size)
{
	if (immediate)
		DIE << "Cant assign value to immidate!";

	uint32_t struct_definition_index = params[0];
	uint32_t struct_instance_index = params[1];

	checkVaribleIndex(struct_definition_index);
	StructCache cache;
	if (instance.structCache.find(struct_definition_index) != instance.structCache.end())
		cache = instance.structCache[struct_definition_index];
	else
		cache = CacheStructDefinition(struct_definition_index);

	freeVarible(struct_instance_index);

	instance.meta[struct_instance_index] = cache.total_size;
	instance.varibles[struct_instance_index] = new uint8_t[cache.total_size];

	std::copy(cache.initial_data, cache.initial_data + cache.total_size, instance.varibles[struct_instance_index]);
}
void VMCore::CHECK_STACK(uint32_t* params, bool immediate, uint8_t arg_size)
{
	Value param_value = makeValue(params[0], immediate, arg_size);
	if (StackEquals(param_value))
		push(iTRUE);
	else
		push(iFALSE);
}
//void VMCore::GET_STRUCT(uint32_t* params, bool immediate, uint8_t arg_size)
//{
//	Value struc = makeValue(params[0], false, arg_size);
//	uint32_t index = params[1];
//	if (struc.length < 2)
//		DIE << "Invalid struct passed";
//	if (struc.data[0] != 0xFF) //simple struct indicator
//		DIE << "Invalid struct passed";
//
//	const uint8_t offset = 2;
//	uint8_t amount_of_fields = struc.data[1];
//
//	if (index > amount_of_fields)
//		DIE << "Invalid struct passed";
//
//	if (struc.length < (amount_of_fields+offset))
//		DIE << "Invalid struct passed";
//
//	size_t sum = 0;
//	for (uint8_t i = 0; i < index; i++)
//	{
//		sum += struc.data[offset+i];
//	}
//
//	if (struc.length < sum)
//		DIE << "Invalid struct passed";
//
//	uint8_t length = struc.data[offset + index];
//
//	uint8_t *data = new uint8_t[length];
//	for (size_t i = 0; i < length; i++)
//	{
//
//	}
//
//	struc.data
//}

#pragma endregion
#pragma region Syscalls
void VMCore::SYS_PAUSE()
{
	uint32_t pause_ms = getUintVar();
	std::this_thread::sleep_for(std::chrono::milliseconds(pause_ms));
}
void VMCore::SYS_CLEAR_CONSOLE()
{
	inputManager.clearScreen();
	//DIE << "syscall SYS_CLEAR not implemented!";
}

void VMCore::SYS_READ()
{
	varible buffer = typed_pop();
	varible val = read_raw();
	typed_setVar(buffer, val);

	//Value buffer = getVar(); //put address on stack that we can write to
	//Value val = read_raw();
	//setVar(buffer, val);
}

void VMCore::SYS_PRINT()
{
	varible var = typed_pop();

	print_raw(var);
}
void VMCore::SYS_PRINTLN()
{
	varible var = typed_pop();

	print_raw(var);
	putchar('\n');
}
void VMCore::SYS_DUMP()
{
	//if (!instance.hasSymbols)
	//	DIE << "Cant dump varible because symbols are not included";

	varible data = typed_pop();
	varible symbol = new BaseValue(data->type_index, data->data, std::min(data->length, (size_t)4));
	if (instance.hasSymbols && data->rest != 0) //no symbol
	{	//if there is a label availible
		symbol->type_index = vt_string;
		symbol->data = instance.symbols.at(data->rest - 1);
		symbol->length = instance.symbols_length.at(data->rest - 1);
	}
	printf("symbol: \"");
	print_raw(symbol);
	printf("\" has value: \"");
	print_raw(data);
	printf("\".");

	delete symbol;
}
//void VMCore::SYS_TO_STRING_UNSIGNED()
//{
//	// Pop the buffer (where result will be stored)
//	Value result_var = getVar();
//	// Pop the number to convert
//	Value number = getVar();
//
//	// Ensure number length is <= 4
//	if (number.length > 4)
//		DIE << "Number too bige!!1!";
//
//	// Convert the number to string
//	uint32_t num = *reinterpret_cast<uint32_t*>(number.data);
//	std::string result_str = std::to_string(num);
//
//	// Prepare the buffer
//	std::vector<uint8_t> buffer(result_str.begin(), result_str.end());
//	uint32_t length = buffer.size();
//	uint8_t* data = new uint8_t[length];
//	std::copy(buffer.begin(), buffer.end(), data);
//
//	// Store the result in `result_var`
//	Value result(data, length, true);
//	setVar(result_var, result);
//}
void VMCore::SYS_TO_STRING_SIGNED()
{
	// Pop the buffer (where result will be stored)
	Value result_var = getVar();
	// Pop the number to convert
	Value number = getVar();

	// Ensure number length is <= 4
	if (number.length > 4)
		DIE << "Number too bige!!1!";

	// Convert the number to string
	int32_t num = *reinterpret_cast<int32_t*>(number.data);
	std::string result_str = std::to_string(num);

	// Prepare the buffer
	std::vector<uint8_t> buffer(result_str.begin(), result_str.end());
	uint32_t length = buffer.size();
	uint8_t* data = new uint8_t[length];
	std::copy(buffer.begin(), buffer.end(), data);

	// Store the result in `result_var`
	Value result(data, length, true);
	setVar(result_var, result);
}
void VMCore::SYS_PARSE()
{
	varible str = typed_pop();
	varible destination = typed_pop();
	BaseValue::ParseString(str, destination);

	auto idk = safe_cast<int>(destination);
}
//void VMCore::SYS_TO_NUMBER_UNSIGNED()
//{
//	// Pop the string to convert
//	Value str_val = getVar();
//
//	// Extract the string from the Value
//	std::string str(str_val.data, str_val.data + str_val.length);
//
//	// Convert to unsigned number
//	try
//	{
//		unsigned long num = std::stoul(str);
//		if (num > std::numeric_limits<uint32_t>::max())
//			throw std::out_of_range("Number too large for uint32_t");
//
//		// Push the result as uint32_t
//		push(static_cast<uint32_t>(num));
//	}
//	catch (const std::exception& e)
//	{
//		DIE << "Invalid unsigned integer string: " << str << ", error: " << e.what();
//	}
//}
void VMCore::SYS_TO_NUMBER_SIGNED()
{
	// Pop the string to convert
	Value str_val = getVar();

	// Extract the string from the Value
	std::string str(str_val.data, str_val.data + str_val.length);

	// Convert to signed number
	try
	{
		long num = std::stol(str);
		if (num < std::numeric_limits<int32_t>::min() || num > std::numeric_limits<int32_t>::max())
			throw std::out_of_range("Number out of range for int32_t");

		// Push the result as int32_t
		push(static_cast<int32_t>(num));
	}
	catch (const std::exception& e)
	{
		DIE << "Invalid signed integer string: " << str << ", error: " << e.what();
	}
}

void VMCore::SYS_INPUT_MODE_READ()
{
	inputManager.setInputModeReading();
}

void VMCore::SYS_INPUT_MODE_WRITE()
{
	inputManager.setInputModePrinting();
}

void VMCore::SYS_INPUT_TO_STRUCT()
{
	Value struct_instance = getVar();
	if (struct_instance.immediate)
		DIE << "struct input can't be used with immediate value";

	StructCache cache;
	GetStructCache(struct_instance, &cache);

	if (struct_instance.length < cache.total_size)
		DIE << "Struct instance was too small";

	for (size_t i = 0; i < cache.offsets.size(); i++)
	{
		uint32_t field_offset = cache.offsets[i];
		uint8_t field_length = cache.lengths[i];
		uint32_t scan_code = binaryApi.CastToUint32(&cache.initial_data[field_offset], field_length);

		bool result = inputManager.getKeyDown(static_cast<int32_t>(scan_code));
		uint8_t *data = binaryApi.CastFromUint32(result, field_length);

		std::copy(data, data+field_length, struct_instance.data+field_offset);
	}
	fflush(stdin);
}

void VMCore::SYS_SET_CONSOLE_CURSOR()
{
	uint32_t y = getUintVar();
	uint32_t x = getUintVar();
	inputManager.setCursor(x, y);
}

void VMCore::SYS_GET_CONSOLE_CURSOR()
{
	int32_t y = 0;
	int32_t x = 0;
	inputManager.getCursor(&x, &y);
	push(y);
	push(x);
}

#pragma endregion
#pragma region Syscall_helpers
void VMCore::print_raw(Value val)
{
	print_raw(val.data, val.length);
}
void VMCore::print_raw(varible var)
{
	if (var->type_index == vt_string) //we have to somehow make it string
	{
		print_raw(var->data/*+offset*/, var->length);
	}
	else
	{
		varible str = BaseValue::ToString(var);
		print_raw(str->data, str->length);
		delete str;
	}
}
void VMCore::print_raw(uint8_t* data, uint32_t length) {
	for (uint32_t i = 0; i < length; ++i)
		putchar(data[i]);  // Print each byte as a character
}
varible VMCore::read_raw() {
	std::vector<uint8_t> buffer;
	int value = getchar();

	while (value >= ' ' && value <= '~')
	{
		buffer.push_back(static_cast<uint8_t>(value));
		value = getchar();
	}

	uint32_t length = buffer.size();


	BaseValue* result = new BaseValue(vt_string, 0);
	result->length = length;
	result->data = new uint8_t[length]; // or malloc if you want C-style
	std::copy(buffer.begin(), buffer.end(), result->data);
	return result;
	//std::memcpy(result->data, str.data(), result->length);




	//return makeValue<char*>();
	//uint8_t* data = new uint8_t[length];
	//std::copy(buffer.begin(), buffer.end(), data);
	// Now `array` points to the raw data, and `length` contains the number of elements
	//return Value(data, length, true);
}
//Value VMCore::read_raw() {
//	std::vector<uint8_t> buffer;
//	int value = getchar();
//
//	while(value >= ' ' && value <= '~')
//	{
//		buffer.push_back(static_cast<uint8_t>(value));
//		value = getchar();
//	}
//
//	uint32_t length = buffer.size();
//	uint8_t* data = new uint8_t[length];
//	std::copy(buffer.begin(), buffer.end(), data);
//	// Now `array` points to the raw data, and `length` contains the number of elements
//	return Value(data, length, true);
//}
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
