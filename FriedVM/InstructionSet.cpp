#include "InstructionSet.h"

std::vector<std::function<void()>> syscall_lookup;
std::vector<INSTRUCTION> opcode_lookup =
{
	{ iEXIT,			"EXIT",          1},
	{ iSYSCALL,			"SYSCALL",       1},
	{ iPUSH,			"PUSH",          1},
	{ iSTORE,			"STORE",         1},
	{ iSET_VAR,			"SET_VAR",       0},
	{ iPOP,				"POP",           0},
	{ iSWAP,			"SWAP",          0},
	{ iDUP,				"DUP",           0},
	{ iMATH,			"MATH",          1},
	{ iINC,				"INC",           1},
	{ iDEC,				"DEC",           1},
	{ iCOMP,			"COMP",          1},
	{ iCHECK_STACK,		"CHECK_STACK",   1},
	{ iNOT,				"NOT",           0},
	{ iJUMP,			"JUMP",          1},
	{ iJUMP_IF,			"JUMP_IF",       1},
	{ iJUMP_IF_STACK,	"JUMP_IF_STACK", 2},
	{ iCALL,			"CALL",          1},
	{ iCALL_IF,			"CALL_IF",       1},
	{ iCALL_IF_STACK,	"CALL_IF_STACK", 2},
	{ iRET,				"RET",           0},
	{ iSET_BUFFER,		"SET_BUFFER",	 1},
	{ iPUSH_BUFFER,		"PUSH_BUFFER",	 1},
	{ iGET_BUFFER,		"GET_BUFFER",    1},
	{ iBUFFER_UTIL,		"BUFFER_UTIL",   1},
	{ iSET_STRUCT,		"SET_STRUCT",    2},
	{ iGET_STRUCT,		"GET_STRUCT",    2},
	{ iCREATE_STRUCT,	"CREATE_STRUCT", 2},

};