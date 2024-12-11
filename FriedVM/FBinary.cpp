#include "FBinary.h"

void FBinary::ParseMagic()
{

	auto magic_size = sizeof(file_magic) / sizeof(*file_magic);
	if (instance.bytecode.size() < (instance.pc + magic_size))
	{
		DIE << "file size was too small, expected more bytes";
		return;
	}

	for (uint8_t i = 0; i < magic_size; i++)
	{
		if (file_magic[i] != instance.bytecode[instance.pc + i])
		{
			DIE << "The input file was not in the correct format";
			return;
		}
	}

	instance.pc += magic_size;
}

INSTRUCTION FBinary::GetInstruction()
{
	if (instance.bytecode.size() < instance.pc)
	{
		DIE << "file was too small expected more instructions";
		return NULL_INSTRUCTION;
	}

	uint8_t hex = instance.bytecode.at(instance.pc);

	//uint8_t opcode = (hex &		0b11111000) >> 3; // first 5 bits are the opcode shift the 0 so we get the actual value
	//uint8_t arg_size = ((hex &	0b00000110) >> 1) + 1; // second 2 bits are the size of the argument (1,2,3 or 4 bytes per argument, no 0)
	//bool immediate = (hex &		0b00000001);      // the last bit is a flag to indicate if the value should be looked up in the const pool
	uint8_t opcode =   (hex	&	0b00011111);			// first 5 bits are the opcode shift the 0 so we get the actual value
	uint8_t arg_size = ((hex &	0b01100000) >> 5) + 1;	// second 2 bits are the size of the argument (1,2,3 or 4 bytes per argument, no 0)
	bool immediate =   (hex &	0b10000000) >> 7;		// the last bit is a flag to indicate if the value should be looked up in the const pool
	if (opcode_lookup.size() >= opcode)
	{
		instance.pc++;
		INSTRUCTION instruction = opcode_lookup.at(opcode);
		instruction.arg_size = arg_size;
		instruction.immediate = immediate;
		return instruction;
	}
	else
	{
		DIE << "Encounterd unknown opcode: " << HEX(opcode) << " at position: " << instance.pc << " no instruction found!";
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

//uint8_t* FBinary::GetInstruction(uint8_t count)
//{
//	uint8_t* buffer = new uint8_t[count];
//	if (instance.bytecode.size() < (instance.pc + count))
//	{
//		DIE << "file size was too small, expected more bytes";
//		return buffer;
//	}
//
//	for (uint8_t i = 0; i < count; i++)
//	{
//		buffer[i] = instance.bytecode[instance.pc + i];
//	}
//	instance.pc += count;
//	return buffer;
//}

uint8_t* FBinary::ReadBytes(uint8_t count)
{
	uint8_t* buffer = new uint8_t[count];
	if (instance.bytecode.size() < (instance.pc+count))
	{
		DIE << "file size was too small, expected more bytes";
		return buffer;
	}

	for (uint8_t i = 0; i < count; i++)
	{
		buffer[i] = instance.bytecode[instance.pc+i];
	}
	instance.pc += count;
	return buffer;
}
