#include "FStack.h"

ByteTag FStack::GetType()
{
	TypeBoundCheck();
	ByteTag value = type_stack.at(stp);
	return value;
}

uint8_t* FStack::PopAndGet()
{
	Pop();
	return Get();
}
uint8_t* FStack::Get()
{
	BoundCheck();
	auto count = ByteTranslator::GetByteCount(GetType());
	sp-=count;
	BoundCheck();
	uint8_t* value = new uint8_t[count];
	for (int i = sp; i < count; ++i) {
		if (i < stack.size()) {
			value[i] = stack[i];
		}
	}
	sp += count; //we didnt actualy change the stack so we need to revert this
	return value;
}
void FStack::Pop()
{
	BoundCheck();
	auto count = ByteTranslator::GetByteCount(GetType());
	sp -= count;
	BoundCheck();
	//uint8_t* value = new uint8_t[count];
	for (int i = sp; i < count; ++i) {
		stack.pop_back();
	}
	TypeBoundCheck();
	stp--; // we need to remove this type we just popped
	type_stack.pop_back();
	return;
}

void FStack::PushTyped(uint8_t* value, ByteTag type)
{
	uint8_t size = ByteTranslator::GetByteCount(type);
	sp += size;
	for (int i = 0; i < size; i++)
	{
		stack.push_back(value[i]);
	}
	stp++;
	type_stack.push_back(type);
}
void FStack::Push(uint8_t* value, INSTRUCTION instruction)
{
	uint8_t size = instruction.arg_size;
	auto type = ByteTranslator::GetByteTag(size);
	sp += size;
	for (int i = 0; i < size; i++)
	{
		stack.push_back(value[i]);
	}
	stp++;
	type_stack.push_back(type);
}

void FStack::BoundCheck()
{
	if (sp <= 0)
	{
		DIE << "Stack underflow! at: " << HEX(instance.pc);
	}
}
void FStack::TypeBoundCheck()
{
	if (stp <= 0)
	{
		DIE << "Stack underflow! at: " << HEX(instance.pc);
	}
}