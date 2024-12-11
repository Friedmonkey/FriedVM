// FriedVM.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "VMCore.h" // Include your VMCore and related classes

//#define DEBUG

// Predefined bytecode array for debug mode
#ifdef DEBUG
uint8_t debugBytecode[] = 
{
	0x46, 0x58, 0x45, 0x31, //magic
	iPUSH, 60, //push 60
	iPUSH,  9, //push  9
	iADD, //add the 2 together, making 69

	iPUSH, 1, // push 1
	iADD, //add 1 to the 69, making it 70

	iPUSH, 70, // push 70

	iEQ, // are the last 2 stack values the same? (put 1 if the same 0 if not)
	iNOT, //reverse it so we get 0 if they were equal (0 is good exit code)

	iEXIT,  //exit with success(0) if they were equal or exit with error 1 if they were not
}; // Example bytecode
#endif


// TODO: Reference additional headers your program requires here.
