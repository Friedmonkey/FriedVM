#pragma once

#include "VMInstance.h"
#include "ByteTranslator.h"
#include "InstructionSet.h"

class FStack : VMInstanceBase
{
public:
	FStack(VMInstance& newInstance) : VMInstanceBase(newInstance)
	{
	}


	ByteTag GetType();
	uint8_t *PopAndGet();
	uint8_t *Get();
	void Pop();
	void PushTyped(uint8_t* value, ByteTag type);
	void Push(uint8_t *value, INSTRUCTION instruction);
private:
	void BoundCheck();
	void TypeBoundCheck();

	uint8_t sp = 0;
	std::vector<uint8_t> stack;


	uint8_t stp = 0;
	std::vector<ByteTag> type_stack;
};