#include "FBinary.h"

void FBinary::ParseMagic()
{
	auto magic_size = sizeof(file_magic) / sizeof(*file_magic);
	for (size_t i = 0; i < magic_size; i++)
	{
		//TODO: implement this
	}
}

INSTRUCTION FBinary::GetInstruction()
{
	if (instance.bytecode.size() < instance.pc)
	{
		DIE << "file was too small expected more instructions";
		return NULL_INSTRUCTION;
	}

	uint8_t opcode = instance.bytecode.at(instance.pc++);
	if (opcode_lookup.size() >= opcode)
	{
		return opcode_lookup.at(opcode);
	}
	else
	{
		DIE << "oops";
		return NULL_INSTRUCTION;
	}
}

uint8_t* FBinary::GetParams(int count)
{
	if (count > maxParamCount)
	{
		DIE << "Amount of paramters requested exeeds the max amount of parameters defined!";
		return NULL;
	}
	return ReadBytes(count);
}

uint8_t* FBinary::ReadBytes(uint8_t count)
{
	uint8_t* paramBuffer = new uint8_t[count];
	if (instance.bytecode.size() < (instance.pc+count))
	{
		DIE << "file size was too small, expected more bytes";
		return paramBuffer;
	}

	for (uint8_t i = 0; i < count; i++)
	{
		paramBuffer[i] = instance.bytecode[instance.pc+i];
	}
	instance.pc += count;
	return paramBuffer;
}
