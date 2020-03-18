/*
 * RegAlloc.h
 * Author: Joshua Cao
 */

#ifndef REGISTER_ALLOCATOR
#define REGISTER_ALLOCATOR

#include "RegAllocStructs.h"
#include "SSA.h"
#include <iostream>

void addOperandToLive(std::list<SSA::Instruction*>& live, SSA::Operand* o);
void insertMoveBeforePhi(SSA::Function* f);

void allocateRegisters(SSA::Function* f);
void allocateRegisters(SSA::Module* ir);

#endif
