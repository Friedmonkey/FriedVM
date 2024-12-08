#pragma once

#include <string>
#include "VMInstance.h"
#include "FBinary.h"

class VMCore : VMInstanceBase
{
public:
	VMCore(VMInstance& newInstance);
	void Parse();
private:
	FBinary binaryApi;
	//FStack stackApi;

	void PUSH(uint8_t params[]);
	void EXIT(uint8_t params[]);
};