#include "VMCore.h"

#define MAKE_EXECUTE(method) [this](uint8_t params[]) { this->method(params); }

VMCore::VMCore(VMInstance& newInstance) : VMInstanceBase(newInstance), binaryApi(newInstance)
{
	opcode_lookup[0x00].execute = MAKE_EXECUTE(PUSH);
	opcode_lookup[0x1A].execute = MAKE_EXECUTE(EXIT);
}

void VMCore::Parse()
{
	binaryApi.ParseMagic();
	for (;;)
	{
		INSTRUCTION instruction = binaryApi.GetInstruction();
		if (instruction.execute == nullptr)
		{
			DIE << "not initialized";
			return;
		}
		if (instruction.paramCount > maxParamCount)
		{
			DIE << "instruction: " << instruction.op_name << " Exeeded the max amount of parameters!";
			return;
		}

		auto params = binaryApi.GetParams(instruction.paramCount);
		instruction.execute(params);
	}
}
void VMCore::PUSH(uint8_t params[])
{
	
}
void VMCore::EXIT(uint8_t params[])
{
	//instance.
	exit(EXIT_SUCCESS);
}

