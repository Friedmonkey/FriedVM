#include "InstructionSet.h"

#define DEFINE_INSTRUCTION(opcode, name, argc) { opcode, { opcode, name, argc}}

std::vector<std::function<void()>> syscall_lookup;
std::unordered_map<uint8_t, INSTRUCTION> opcode_lookup =
{
	DEFINE_INSTRUCTION(iEXIT            ,"EXIT",          1),
	DEFINE_INSTRUCTION(iSYSCALL         ,"SYSCALL",       1),
	DEFINE_INSTRUCTION(iPUSH            ,"PUSH",          1),
	DEFINE_INSTRUCTION(iSTORE           ,"STORE",         1),
	DEFINE_INSTRUCTION(iSET_VAR         ,"SET_VAR",       0),
	DEFINE_INSTRUCTION(iPOP             ,"POP",           0),
	DEFINE_INSTRUCTION(iSWAP            ,"SWAP",          0),
	DEFINE_INSTRUCTION(iDUP             ,"DUP",           0),
	DEFINE_INSTRUCTION(iMATH            ,"MATH",          1),
	DEFINE_INSTRUCTION(iINC             ,"INC",           1),
	DEFINE_INSTRUCTION(iDEC             ,"DEC",           1),
	DEFINE_INSTRUCTION(iCOMP            ,"COMP",          1),
	DEFINE_INSTRUCTION(iCHECK_STACK     ,"CHECK_STACK",   1),
	DEFINE_INSTRUCTION(iNOT             ,"NOT",           0),
	DEFINE_INSTRUCTION(iJUMP            ,"JUMP",          1),
	DEFINE_INSTRUCTION(iJUMP_IF         ,"JUMP_IF",       1),
	DEFINE_INSTRUCTION(iJUMP_IF_STACK   ,"JUMP_IF_STACK", 2),
	DEFINE_INSTRUCTION(iCALL            ,"CALL",          1),
	DEFINE_INSTRUCTION(iCALL_IF         ,"CALL_IF",       1),
	DEFINE_INSTRUCTION(iCALL_IF_STACK   ,"CALL_IF_STACK", 2),
	DEFINE_INSTRUCTION(iRET             ,"RET",           0),
	DEFINE_INSTRUCTION(iSET_BUFFER      ,"SET_BUFFER",	 1),
	DEFINE_INSTRUCTION(iPUSH_BUFFER     ,"PUSH_BUFFER",	 1),
	DEFINE_INSTRUCTION(iGET_BUFFER      ,"GET_BUFFER",    1),
	DEFINE_INSTRUCTION(iBUFFER_UTIL     ,"BUFFER_UTIL",   1),
	DEFINE_INSTRUCTION(iSET_STRUCT      ,"SET_STRUCT",    2),
	DEFINE_INSTRUCTION(iGET_STRUCT      ,"GET_STRUCT",    2),
	DEFINE_INSTRUCTION(iCREATE_STRUCT   ,"CREATE_STRUCT", 2),
};