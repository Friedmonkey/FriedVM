#include "FBinary.h"

//parse the magic + its version and if it says it has symbols included
// version is either 1,2,3,4,5,6,7,8,9
//the same version but with symbols is !,@,#,$,%,^,&,*,(
void FBinary::ParseMagic()
{
	bool hasSymbols = false;
	auto magic_size = sizeof(file_magic) / sizeof(*file_magic);
	magic_size++; //the version
	if (instance.bytecode.size() < (instance.pc + magic_size))
	{
		DIE << "file size was too small, expected more bytes";
	}

	for (uint8_t i = 0; i < magic_size; i++)
	{
		if (i == magic_size-1) //the version
		{
			constexpr uint8_t size_map[4] = { 1, 2, 4, 8 };
			uint8_t version_byte = instance.bytecode[instance.pc + i];
			uint8_t header_size = size_map[(version_byte & 0b11000000) >> 6];
			uint8_t meta_size = size_map[(version_byte & 0b00110000) >> 4];
			hasSymbols = (version_byte & 0b00001000) >> 3;
			uint8_t version = (version_byte & 0b00000111);

			instance.header_size = header_size;
			instance.meta_size = meta_size;
			instance.hasSymbols = hasSymbols;
			instance.version = version;
		}
		else
		{
			if (file_magic[i] != instance.bytecode[instance.pc + i])
			{
				DIE << "The input file was not in the correct format";
			}
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
	uint8_t opcode =   (hex &	0b00011111);			// first 5 bits are the opcode shift the 0 so we get the actual value
	uint8_t arg_size = ((hex &	0b01100000) >> 5) + 1;	// second 2 bits are the size of the argument (1,2,3 or 4 bytes per argument, no 0)
	bool isAddr =   (hex &	0b10000000) >> 7;		// the last bit is a flag to indicate if the value should be looked up in the const pool
	if (opcode_lookup.size() >= opcode)
	{
		instance.pc++;
		INSTRUCTION instruction = opcode_lookup.at(opcode);
		instruction.arg_size = arg_size;
		instruction.immediate = !isAddr;
		return instruction;
	}
	else
	{
		DIE << "Encounterd unknown opcode: " << HEX(opcode) << " at position: " << instance.pc << " no instruction found!";
		return NULL_INSTRUCTION;
	}
}

uint32_t* FBinary::GetParams(INSTRUCTION &instruction)
{
	if (instruction.paramCount > maxParamCount)
	{
		DIE << "Amount of paramters requested exeeds the max amount of parameters defined!";
		return NULL;
	}

	uint32_t *params = new uint32_t[instruction.paramCount];
	for (int i = 0; i < instruction.paramCount; i++)
	{
		auto bytes = ReadBytes(instruction.arg_size);
		params[i] = CastToUint32(bytes, instruction.arg_size);
	}

	return params;
}

uint64_t FBinary::ParseMeta()
{
	return CastToUint64(ReadBytes(instance.meta_size), instance.meta_size);
}
uint64_t FBinary::ParseAddress()
{
	return CastToUint64(ReadBytes(instance.header_size), instance.header_size);
}

uint8_t* FBinary::CastFromUint32(const uint32_t uint32, size_t count)
{
	uint8_t* buffer = new uint8_t[count];  // Allocate memory for the byte array

	for (size_t i = 0; i < count; ++i)
	{
		// Extract each byte by shifting and masking
		buffer[i] = static_cast<uint8_t>((uint32 >> (8 * i)) & 0xFF); // Shift and mask to get the byte
	}

	return buffer;
}
uint32_t FBinary::CastToUint32(const uint8_t* byteArray, size_t count)
{
	uint32_t result = 0;

	for (size_t i = 0; i < count; ++i)
	{
		result |= static_cast<uint32_t>(byteArray[i]) << (8 * i); // Shift and OR
	}

	return result;
}
uint64_t FBinary::CastToUint64(const uint8_t* byteArray, size_t count)
{
	uint64_t result = 0;

	for (size_t i = 0; i < count; ++i)
	{
		result |= static_cast<uint64_t>(byteArray[i]) << (8 * i); // Shift and OR
	}

	return result;
}

uint8_t* FBinary::ReadBytes(uint8_t count)
{
	uint8_t* buffer = new uint8_t[count];
	if (instance.bytecode.size() < (instance.pc+count))
	{
		DIE << "file size was too small (" << NUM(instance.bytecode.size()) << "), expected more bytes (" << NUM(instance.pc + count) << ")";
		return buffer;
	}

	for (uint8_t i = 0; i < count; i++)
	{
		buffer[i] = instance.bytecode[instance.pc+i];
	}
	instance.pc += count;
	return buffer;
}
