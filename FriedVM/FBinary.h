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
	uint32_t* GetParams(INSTRUCTION& instruction);
	uint64_t ParseMeta();
	uint64_t ParseAddress();
	uint8_t* CastFromUint32(const uint32_t uint32, size_t count);
	uint32_t CastToUint32(const uint8_t* byteArray, size_t count);
private:
	uint64_t CastToUint64(const uint8_t* byteArray, size_t count);
	uint8_t* ReadBytes(uint8_t count);
};