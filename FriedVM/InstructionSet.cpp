#include "InstructionSet.h"

std::vector<std::function<void()>> syscall_lookup;
std::vector<INSTRUCTION> opcode_lookup =
{
	{ iPUSH, "PUSH",		1 },
	{ iPOP, "POP",			0 },
	{ iDUP, "DUP",			0 },
	{ iMATH, "MATH",		1 },
	{ iAND, "AND",			0 },
	{ iOR, "OR",			0 },
	{ iNOT, "NOT",			0 },
	{ iCOMP, "COMP",		1 },
	{ iJUMP, "JUMP",		1 },
	{ iJUMP_IF, "JUMP_IF",	2 },
	{ iCALL, "CALL",		1 },
	{ iCALL_IF, "CALL_IF",	2 },
	{ iRET, "RET",			0 },
	{ iSYSCALL, "SYSCALL",	1 },
	{ iEXIT, "EXIT",		0 },

	{ iVAR, "VAR",				1 },
	{ iPOP_TO_VAR, "POP_TO_VAR",1 },
	{ iVAR_MOV, "VAR_MOV",		2 },
	{ iVAR_PUSH, "VAR_PUSH",	2 },
	{ iVAR_POP, "VAR_POP",		1 },
	{ iSTRUCT_SET, "STRUCT_SET",2 },
	{ iSTRUCT_GET, "STRUCT_GET",2 },
};