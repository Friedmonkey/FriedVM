﻿# CMakeList.txt : CMake project for FriedVM, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (FriedVM "FriedVM.cpp" "FriedVM.h" "VMCore.h" "VMCore.cpp" "InstructionSet.h" "Logger.h" "FBinary.h" "FBinary.cpp" "VMInstance.h" "InstructionSet.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET FriedVM PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
