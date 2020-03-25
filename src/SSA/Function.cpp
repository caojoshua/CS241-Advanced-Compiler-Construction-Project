/*
 * Function.cpp
 * Author: Joshua Cao
 */

#include "Operand.h"
#include "Instruction.h"
#include "Function.h"
#include "Module.h"

SSA::Function::~Function()
{
	for (BasicBlock* bb : BBs)
	{
		delete bb;
	}
//	// temporary solution to delete operands, since freeing operands
//	// from instructions cause double frees
//	for (SSA::Operand* o : ops)
//	{
//		delete o;
//	}
}

void SSA::Function::emit(BasicBlock *bb)
{
	bb->setParent(this);
	BBs.push_back(bb);
}

std::string SSA::Function::getName()
{
	return name;
}

std::list<SSA::BasicBlock*> SSA::Function::getBBs()
{
	return BBs;
}

SSA::Module* SSA::Function::getParent() const
{
	return parent;
}

bool SSA::Function::isVoid() const
{
	return isVoidReturn;
}

int SSA::Function::getLocalVariableOffset() const
{
	return localVariableOffset;
}

void SSA::Function::setIsVoid(bool isVoid)
{
	isVoidReturn = isVoid;
}

void SSA::Function::setLocalVariableOffset(int i)
{
	localVariableOffset = i;
}

int SSA::Function::resetLineIds()
{
	uint lineId = 0;
	for (BasicBlock* b : BBs)
	{
		for (Instruction* i : b->getInstructions())
		{
			i->setId(lineId++);
		}
	}
	return lineId;
}

void SSA::Function::resetRegs()
{
	for (BasicBlock* b : BBs)
	{
		for (Instruction* i : b->getInstructions())
		{
			i->setReg(-1);
		}
	}
}
