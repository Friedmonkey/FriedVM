#include "VMCore.h"

void VMCore::Parse()
{
	binaryApi.ParseMagic();

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

	}
	instance.declare_size--; //size not count

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


	std::vector<varible> execParams;
	execParams.reserve(maxParamCount);

	while (instance.ProgramIndex < Statements.size())
	{
		auto& statement = Statements[instance.ProgramIndex];

		bool hasInterpolated = false;

		// first check if any param is interpolated
		for (size_t i = 0; i < statement.Instruction.paramCount; ++i) {
			if (statement.params[i]->type_index == vt_interpolated_string) {
				hasInterpolated = true;
				break;
			}
		}

		if (hasInterpolated) {
			execParams.clear(); // reuse vector
			for (size_t i = 0; i < statement.Instruction.paramCount; ++i) {
				auto& p = statement.params[i];
				if (p->type_index == vt_interpolated_string) {
					execParams.push_back(resolveInterpolated(p)); // returns a BaseValue* or varible
				}
				else {
					execParams.push_back(p);
				}
			}
			bool advanceProgramIndex = statement.Instruction.execute(execParams.data());
			if (advanceProgramIndex)
				instance.ProgramIndex++;
		}
		else {
			bool advanceProgramIndex = statement.Instruction.execute(statement.params);
			if (advanceProgramIndex)
				instance.ProgramIndex++;
		}
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
		byte = var->data[(*offset)++];
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


template<typename T>
T VMCore::safe_cast(varible var)
{
	return BaseValue::getValue<T>(var);
}
varible VMCore::resolveInterpolated(varible interpolated_string)
{
	//if it is made up of only const strings then there is no point reinterpolating it every time
	//so if all are const we will bake and basicly overwrite it.
	//that we set the origional interpolated_string to a normal string of what it's would be
	bool bake = true; 

	std::vector<varible> composition;
	composition.reserve(8); // avoid reallocs, tweak as needed

	uint64_t offset = 0;


	uint64_t totalSize = 0;
	while (offset < interpolated_string->length) //while there are still vlq's left to read
	{
		auto index = GetVarVLQ(interpolated_string, &offset);
		varible var = instance.typed_varibles.at(index);
		if (!var->isConst)
		{	//we have a mutable string which could change, so we cant bake it anymore
			bake = false;
		}
		
		composition.push_back(var);
		totalSize += var->length;
	}

	// Allocate a buffer big enough for the final string
	std::string output;
	output.reserve(totalSize);

	// Concatenate all string parts
	for (auto& var : composition)
	{
		if (var->type_index == vt_interpolated_string)
		{
			output.append(resolveInterpolated(var)->toString());
		}
		else
		{
			output.append(var->toString());
		}

	}



	// Return a new runtime string variable
	return BaseValue::makeString(output);


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




void VMCore::syscall(uint32_t index)
{
	if (index > syscall_lookup.size())
		DIE << "Syscall at index "<< NUM(index) << " does not exist!";
	syscall_lookup.at(index)();
}








void VMCore::typed_setVar(varible reference, varible newValue) {


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


}
bool VMCore::Compare(uint8_t compMode, varible val1, varible val2)
{
	if (!(val1->isNumber() && val2->isNumber()))
	{
		if (compMode != cmEQ && compMode != cmNEQ)
			DIE << "unable to compare non-numbers unequally";
		bool result = BaseValue::Equals(val1, val2);
		if (compMode == cmNEQ)
			result = !result;

		return result;
	}
	varible result;
	switch (compMode) {
	case cmEQ: result = BaseValue::computeNumbers(val1, val2, BaseValue::EQCompOperation{}); break;
	case cmNEQ: result = BaseValue::computeNumbers(val1, val2, BaseValue::NEQCompOperation{}); break;
	case cmGT: result = BaseValue::computeNumbers(val1, val2, BaseValue::GTCompOperation{}); break;
	case cmGTE: result = BaseValue::computeNumbers(val1, val2, BaseValue::GTECompOperation{}); break;
	case cmLT: result = BaseValue::computeNumbers(val1, val2, BaseValue::LTCompOperation{}); break;
	case cmLTE: result = BaseValue::computeNumbers(val1, val2, BaseValue::LTECompOperation{}); break;
	default:
		DIE << "Compare mode with index " << HEX(compMode) << " does not exist!";
	}

	return (result->isTruthy());
}

void VMCore::Jump(varible offset)
{
	if (offset->type_index != vt_label)
		DIE << "Jump expected a label but got type " << HEX(offset->type_index) << " instead";

	uint64_t idx = safe_cast<uint64_t>(offset);
	instance.ProgramIndex = idx;
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

	opcode_lookup[iSET_CASE].execute = MAKE_EXECUTE(typed_SET_CASE);
	opcode_lookup[iJUMP_IF_CASE].execute = MAKE_EXECUTE(typed_JUMP_IF_CASE);

	opcode_lookup[iCALL].execute = MAKE_EXECUTE(typed_CALL);
	opcode_lookup[iCALL_IF].execute = MAKE_EXECUTE(typed_CALL_IF);
	opcode_lookup[iCALL_IF_STACK].execute = MAKE_EXECUTE(typed_CALL_IF_STACK);

	opcode_lookup[iRET].execute = MAKE_EXECUTE(typed_RET);

#pragma endregion
#pragma region Syscalls
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_CLEAR_CONSOLE));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_READ));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_PRINT));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_PRINTLN));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_DUMP));

	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_PARSE));

	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_INPUT_MODE_READ));
	syscall_lookup.push_back(MAKE_SYS_EXECUTE(SYS_INPUT_MODE_WRITE));
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

	uint8_t math_mode = safe_cast<uint8_t>(params[0]);



	if (math_mode != mmINC && math_mode != mmDEC)
		val2 = typed_pop();
	val1 = typed_pop();

	varible result = 0;
	varible one = BaseValue::makeValue(vt_uint32_t, 1);

	switch (math_mode) {
	case mmADD: result = BaseValue::computeNumbers(val1, val2, BaseValue::AddOperation{}); break;
	case mmSUB: result = BaseValue::computeNumbers(val1, val2, BaseValue::SubOperation{}); break;
	case mmINC: result = BaseValue::computeNumbers(val1, one, BaseValue::AddOperation{}); break;
	case mmDEC: result = BaseValue::computeNumbers(val1, one, BaseValue::SubOperation{}); break;
	case mmMUL: result = BaseValue::computeNumbers(val1, val2, BaseValue::MulOperation{}); break;
	case mmDIV: result = BaseValue::computeNumbers(val1, val2, BaseValue::DivOperation{}); break;

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




bool VMCore::typed_COMP(varible* params)
{
	uint8_t compare_mode = safe_cast<uint8_t>(params[0]);
	varible val2 = typed_pop();
	varible val1 = typed_pop(); 

	bool result = Compare(compare_mode, val1, val2);
	
	if (result)
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

bool VMCore::typed_SET_CASE(varible* params)
{
	caseValue = params[0];
	return true;
}
bool VMCore::typed_SET_CASE_MODE(varible* params)
{
	caseCompareMode = safe_cast<uint8_t>(params[0]);
	return true;
}

bool VMCore::typed_JUMP_IF_CASE(varible* params)
{
	bool shouldJump = Compare(caseCompareMode, caseValue, params[0]);
	if (shouldJump)
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









#pragma endregion
#pragma region Syscalls

void VMCore::SYS_CLEAR_CONSOLE()
{
	inputManager.clearScreen();

}

void VMCore::SYS_READ()
{
	varible val = read_raw();
	typed_push(val);
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


void VMCore::SYS_PARSE()
{
	varible destination = typed_pop();
	varible str = typed_pop();
	BaseValue::ParseString(str, destination);

	auto idk = safe_cast<int>(destination);
}



void VMCore::SYS_INPUT_MODE_READ()
{
	inputManager.setInputModeReading();
}

void VMCore::SYS_INPUT_MODE_WRITE()
{
	inputManager.setInputModePrinting();
}







#pragma endregion
#pragma region Syscall_helpers

void VMCore::print_raw(varible var)
{
	if (var->type_index == vt_string) //we have to somehow make it string
	{
		print_raw(var->data, var->length);
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


	BaseValue* result = new BaseValue(vt_string, length);

	std::copy(buffer.begin(), buffer.end(), result->data);
	return result;

}

