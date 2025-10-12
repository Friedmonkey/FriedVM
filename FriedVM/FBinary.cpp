#include "FBinary.h"

//parse the magic + its version and if it says it has symbols included
// version is either 1,2,3,4,5,6,7,8,9
//the same version but with symbols is !,@,#,$,%,^,&,*,(
void FBinary::ParseMagic()
{
	//bool hasSymbols = false;
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
			//constexpr uint8_t size_map[4] = { 1, 2, 4, 8 };
			uint8_t info_byte = instance.bytecode[instance.pc + i];

			//uint8_t header_size = size_map[(info_byte & 0b11000000) >> 6];
			//uint8_t meta_size = size_map[(info_byte &   0b00110000) >> 4];
			//uint8_t emptyVar_size = size_map[(info_byte&0b00001100) >> 2];
			bool compact = (info_byte & 0b01000000) >> 6;
			bool hasSymbols = (info_byte & 0b10000000) >> 7;
			uint8_t version = (info_byte & 0b00111111);

			//instance.header_size = header_size;
			//instance.meta_size = meta_size;
			//instance.emptyVar_size = emptyVar_size;
			instance.compact = compact;
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
		return NOOP_INSTRUCTION;
	}

	uint8_t opcode = instance.bytecode.at(instance.pc);

	//uint8_t opcode = (hex &		0b11111000) >> 3; // first 5 bits are the opcode shift the 0 so we get the actual value
	//uint8_t arg_size = ((hex &	0b00000110) >> 1) + 1; // second 2 bits are the size of the argument (1,2,3 or 4 bytes per argument, no 0)
	//bool immediate = (hex &		0b00000001);      // the last bit is a flag to indicate if the value should be looked up in the const pool
	//uint8_t opcode =   (hex &	0b00011111);			// first 5 bits are the opcode shift the 0 so we get the actual value
	//uint8_t arg_size = ((hex &	0b01100000) >> 5) + 1;	// second 2 bits are the size of the argument (1,2,3 or 4 bytes per argument, no 0)
	///bool isAddr =   (hex &	0b10000000) >> 7;		// the last bit is a flag to indicate if the value should be looked up in the const pool
	if (opcode_lookup.contains(opcode))
	{
		instance.pc++;
		INSTRUCTION instruction = opcode_lookup.at(opcode);
		//instruction.arg_size = arg_size;
		//instruction.immediate = !isAddr;
		return instruction;
	}
	else
	{
		DIE << "Encounterd unknown opcode: " << HEX(opcode) << " at position: " << instance.pc << " no instruction found!";
		return NOOP_INSTRUCTION;
	}
}

uint64_t FBinary::VLQ()
{
	uint64_t value = 0;
	uint8_t shift = 0;
	uint8_t byte = 0;

	do
	{
		byte = GetByte();
		value |= (uint64_t)(byte & 0x7F) << shift; // Mask out MSB and shift
		shift += 7;

		if (shift >= 64) // Prevent overflow
		{
			DIE << "VLQ decoding error: shift exceeded 64 bits, possibly malformed data.";
		}

	} while (byte & 0x80); // Continue if MSB is 1

	return value;
}

varible* FBinary::GetParams(INSTRUCTION& instruction)
{
	if (instruction.paramCount > maxParamCount)
	{
		DIE << "Amount of paramters requested exeeds the max amount of parameters defined!";
		return NULL;
	}

	varible* params = new varible[instruction.paramCount];
	for (int i = 0; i < instruction.paramCount; i++)
	{
		params[i] = instance.typed_varibles.at(VLQ());
		//auto bytes = ReadBytes(instruction.arg_size);
		//params[i] = CastToUint32(bytes, instruction.arg_size);
		// = (uint32_t)VLQ();
	}

	return params;
}

uint64_t FBinary::offsetted_VLQ(uint64_t* offset)
{
	uint64_t value = 0;
	uint8_t shift = 0;
	uint8_t byte = 0;

	do
	{
		byte = GetByte(offset);
		value |= (uint64_t)(byte & 0x7F) << shift; // Mask out MSB and shift
		shift += 7;

		if (shift >= 64) // Prevent overflow
		{
			DIE << "VLQ decoding error: shift exceeded 64 bits, possibly malformed data.";
		}

	} while (byte & 0x80); // Continue if MSB is 1

	return value;
}

void FBinary::getVaribleTypeSize(uint64_t* position, varible varible)
{
	uint64_t value = offsetted_VLQ(position);
	delete varible->data;
	varible->data = CastFromUint64(value, varible->length);

}
uint64_t FBinary::getComplexTypeSize(ValueType type, uint64_t *position) {
	switch (type) {
	case vt_raw: return offsetted_VLQ(position);
	case vt_string: return offsetted_VLQ(position);
	case vt_interpolated_string: return offsetted_VLQ(position);

	case vt_array: return 0;
	default: DIE << "complex type size is either not handled yet or not supported for type " << HEX(type);
	}
}
void FBinary::FillData(uint64_t *position, varible varible)
{
	//size_t pos = (*position) + ;
	if (varible->type_index == vt_label)
	{
		getVaribleTypeSize(position, varible);
	}
	else if (IsComplexType(varible->type_index))
	{
		uint64_t size = getComplexTypeSize(varible->type_index, position);
		varible->length = size;
		//size_t post_type_size = 0;//BaseValue::getTypeSize(varible->type_index);
		//size_t size = varible->length + post_type_size;
		uint8_t* buffer = new uint8_t[size];

		for (size_t i = 0; i < size; i++)
		{
			buffer[i] = instance.bytecode[*position + i];
		}
		*position += size;

		delete[] varible->data;
		varible->data = buffer;
	}
	else
	{
		size_t size = BaseValue::getTypeSize(varible->type_index);
		bool isCorrectSize = varible->length == size;

		uint8_t* buffer = isCorrectSize ? varible->data : new uint8_t[size];

		for (size_t i = 0; i < size; i++)
		{
			buffer[i] = instance.bytecode[*position + i];
		}
		*position += size;


		if (!isCorrectSize)
		{	//we need to swap te buffers
			delete[] varible->data;
			varible->data = buffer;
		}
	}




}
uint64_t FBinary::ParseEmptyVarCount()
{
	return CastToUint64(ReadBytes(instance.emptyVar_size), instance.emptyVar_size);
}

varible FBinary::ParseTypeByte(std::vector<uint8_t> &complex_buffer, bool canBeComplex)
{
	uint8_t byte = GetByte();

	bool isConst = ((byte & vt_constant_flag_mask) != 0);
	ValueType type_byte = static_cast<ValueType>(byte & ~vt_constant_flag_mask);


	{
		varible val = BaseValue::createValue(type_byte);
		val->isConst = isConst;
		return val;
	}

}
bool FBinary::IsComplexType(ValueType vt)
{
	switch (vt)
	{
	case vt_string:
	case vt_interpolated_string:
	case vt_complex_type:
	case vt_struct:
	case vt_array:
		return true;
	case vt_pointer:
		return true;
	default:
		return false;
		break;
	}
	return false;
}
bool FBinary::IsHeaderComplexType(ValueType vt)
{
	switch (vt)
	{
	case vt_lazy:
	case vt_complex_type:
	case vt_struct:
		return true;
	case vt_pointer:
		return true;
	default:
		return false;
		break;
	}
	return false;
}
uint64_t FBinary::ParseAddress()
{
	return VLQ();
	//return CastToUint64(ReadBytes(instance.header_size), instance.header_size);
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
	if (count > 4)
		DIE << "Unsupported length for number, max is 4 bytes got " << NUM(count) << " instead!";

	uint32_t result = 0;

	for (size_t i = 0; i < count; ++i)
	{
		result |= static_cast<uint32_t>(byteArray[i]) << (8 * i); // Shift and OR
	}

	return result;
}
uint8_t* FBinary::CastFromUint64(const uint64_t uint64, size_t count)
{
	uint8_t* buffer = new uint8_t[count];  // Allocate memory for the byte array

	for (size_t i = 0; i < count; ++i)
	{
		// Extract each byte by shifting and masking
		buffer[i] = static_cast<uint8_t>((uint64 >> (8 * i)) & 0xFF); // Shift and mask to get the byte
	}

	return buffer;
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

uint8_t FBinary::GetByte(uint64_t *position)
{
	if (instance.bytecode.size() < (*position + 1))
	{
		DIE << "file size was too small (" << NUM(instance.bytecode.size()) << "), expected more bytes (" << NUM(*position + 1) << ")";
	}
	uint8_t byte = instance.bytecode[*position];
	*position = *position + 1;
	return byte;
}
uint8_t FBinary::GetByte()
{
	if (instance.bytecode.size() < (instance.pc + 1))
	{
		DIE << "file size was too small (" << NUM(instance.bytecode.size()) << "), expected more bytes (" << NUM(instance.pc + 1) << ")";
	}
	return instance.bytecode[instance.pc++];
}

uint8_t* FBinary::ReadBytes(uint8_t count)
{
	if (instance.bytecode.size() < (instance.pc+count))
	{
		DIE << "file size was too small (" << NUM(instance.bytecode.size()) << "), expected more bytes (" << NUM(instance.pc + count) << ")";
	}

	uint8_t* buffer = new uint8_t[count];

	for (uint8_t i = 0; i < count; i++)
	{
		buffer[i] = instance.bytecode[instance.pc+i];
	}
	instance.pc += count;
	return buffer;
}
