/*
 * RegAlloc.h
 * Author: Joshua Cao
 */

#ifndef REGISTER_ALLOCATOR
#define REGISTER_ALLOCATOR

#include <RegAllocStructs.h>
#include "SSA.h"
#include <iostream>

void addOperandToLive(std::list<SSA::Instruction*>& live, SSA::Operand* o);
IntervalList buildIntervals(SSA::Func* f);

void allocateRegister(SSA::Func* f);
void allocateRegisters(SSA::IntermediateRepresentation ir);

#endif