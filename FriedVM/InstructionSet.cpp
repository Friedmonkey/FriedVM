#include "InstructionSet.h"

std::vector<std::function<void()>> syscall_lookup;
std::vector<INSTRUCTION> opcode_lookup =
{
	{ iPUSH, "PUSH",		1 },
	{ iPOP, "POP",			0 },
	{ iDUP, "DUP",			0 },
	{ iVAR, "VAR",			2 },
	{ iGET_VAR, "GET_VAR",	1 },
	{ iPOP_VAR, "POP_VAR",	1 },
	{ iPSH_VAR, "PSH_VAR",	1 },
	{ iMOV_VAR, "MOV_VAR",	2 },
	{ iDEL, "DEL",			1 },
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
};