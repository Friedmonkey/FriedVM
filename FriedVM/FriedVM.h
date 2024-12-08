// FriedVM.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "VMCore.h" // Include your VMCore and related classes

#define DEBUG

// Predefined bytecode array for debug mode
#ifdef DEBUG
uint8_t debugBytecode[] = { 0x1A }; // Example bytecode
#endif


// TODO: Reference additional headers your program requires here.
