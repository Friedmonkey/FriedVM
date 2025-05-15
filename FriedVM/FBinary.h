#pragma once
#include "VMInstance.h"
#include "InstructionSet.h"

class FBinary : VMInstanceBase
{
public:
	FBinary(VMInstance& newInstance) : VMInstanceBase(newInstance)
	{
	}
	void ParseMagic();
	INSTRUCTION GetInstruction();
	uint64_t VLQ();
	varible* GetParams(INSTRUCTION& instruction);
	uint64_t offsetted_VLQ(uint64_t* offset);
	void getVaribleTypeSize(uint64_t* position, varible varible);
	uint64_t getComplexTypeSize(ValueType type, uint64_t* position);
	//uint32_t* GetParams(INSTRUCTION& instruction);
	void FillData(uint64_t* position, varible varible);
	uint64_t ParseEmptyVarCount();
	varible ParseTypeByte(std::vector<uint8_t>& complex_buffer, bool canBeComplex = true);
	bool IsComplexType(ValueType vt);
	bool IsHeaderComplexType(ValueType vt);
	uint64_t ParseAddress();
	uint8_t* CastFromUint32(const uint32_t uint32, size_t count);
	uint32_t CastToUint32(const uint8_t* byteArray, size_t count);
	uint8_t* CastFromUint64(const uint64_t uint64, size_t count);
private:
	uint64_t CastToUint64(const uint8_t* byteArray, size_t count);
	uint8_t GetByte(uint64_t* position);
	uint8_t GetByte();
	uint8_t* ReadBytes(uint8_t count);
};