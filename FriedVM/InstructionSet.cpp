#include "InstructionSet.h"

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
	{ iLS, "LS",			0 },
	{ iJMP, "JMP",			1 },
	{ iJMP_IF, "JMP_IF",	1 },
	{ iCALL, "CALL",		1 },
	{ iCALL_IF, "CALL_IF",	1 },
	{ iRET, "RET",			0 },
	{ iSYSCALL, "SYSCALL",	1 },
	{ iEXIT, "EXIT",		0 },
};