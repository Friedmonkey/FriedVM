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
	uint8_t* GetParams(int count);
private:
	uint8_t* ReadBytes(uint8_t count);
};