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
	{ iADD, "ADD",			0 },
	{ iSUB, "SUB",			0 },
	{ iMUL, "MUL",			0 },
	{ iDIV, "DIV",			0 },
	{ iAND, "AND",			0 },
	{ iOR, "OR",			0 },
	{ iNOT, "NOT",			0 },
	{ iEQ, "EQ",			0 },
	{ iNEQ, "NEQ",			0 },
	{ iGT, "GT",			0 },
	{ iGTEQ, "GTEQ",		0 },
	{ iLT, "LT",			0 },
	{ iLTEQ, "LTEQ",		0 },
	{ iJUMP, "JUMP",		1 },
	{ iJUMP_IF, "JUMP_IF",	1 },
	{ iCALL, "CALL",		1 },
	{ iCALL_IF, "CALL_IF",	1 },
	{ iRET, "RET",			0 },
	{ iSYSCALL, "SYSCALL",	1 },
	{ iEXIT, "EXIT",		0 },

	{ iJUMP_IF_STACK, "JUMP_IF_STACK",		2 },
	{ iINC, "INC",			0 },
	{ iDEC, "DEC",			0 },
};