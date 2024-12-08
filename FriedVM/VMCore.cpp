#include "VMCore.h"

VMCore::VMCore(uint8_t progamCounter)
{
	pc = progamCounter;
}

bool VMCore::ReadMagic()
{
	auto magic_size = sizeof(file_magic) / sizeof(file_magic[0]);
	for (size_t i = 0; i < magic_size; i++)
	{
		i++;
	}
}

