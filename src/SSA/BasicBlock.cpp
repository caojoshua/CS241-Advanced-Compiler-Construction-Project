/*
 * BasicBlock.cpp
 * Author: Joshua Cao
 */

#include "BasicBlock.h"

#include "Operand.h"
#include "Instruction.h"
#include "Function.h"
#include "Module.h"

std::list<SSA::Instruction*>::iterator SSA::BasicBlock::getInstructionIter(Instruction* i)
{
	std::list<SSA::Instruction*>::iterator iter;
	for (iter = instructions.begin(); iter != instructions.end(); ++iter)
	{
		if (*iter == i)
		{
			return iter;
		}
	}
	return iter;
}

SSA::BasicBlock::~BasicBlock()
{
	for (Instruction* i : instructions)
	{
		if (i)
		{
			delete i;
		}
	}
}

SSA::Function* SSA::BasicBlock::getParent() const
{
	return parent;
}

void SSA::BasicBlock::setParent(Function* f)
{
	parent = f;
}

void SSA::BasicBlock::emit(Instruction *ins)
{
	ins->setParent(this);
	instructions.push_back(ins);
	parent->getParent()->addOperand(ins->getOperand1());
	parent->getParent()->addOperand(ins->getOperand2());
}

void SSA::BasicBlock::emit(std::list<Instruction*> ins)
{
	for (Instruction* i : ins)
	{
		emit(i);
	}
}

void SSA::BasicBlock::emitFront(Instruction *ins)
{
	ins->setParent(this);
	instructions.push_front(ins);
	parent->getParent()->addOperand(ins->getOperand1());
	parent->getParent()->addOperand(ins->getOperand2());
}

void SSA::BasicBlock::emitBefore(Instruction* x, Instruction* y)
{
	auto iter = getInstructionIter(y);
	if (iter != instructions.end())
	{
		x->setParent(this);
		instructions.insert(iter, x);
		parent->getParent()->addOperand(x->getOperand1());
		parent->getParent()->addOperand(x->getOperand2());
	}
}

void SSA::BasicBlock::emitAfter(Instruction* x, Instruction* y)
{
	auto iter = getInstructionIter(y);
	if (iter != instructions.end())
	{
		x->setParent(this);
		instructions.insert(++iter, x);
		parent->getParent()->addOperand(x->getOperand1());
		parent->getParent()->addOperand(x->getOperand2());
	}
}

std::list<SSA::Instruction*>& SSA::BasicBlock::getInstructions()
{
	return instructions;
}

void SSA::BasicBlock::addPredecessor(BasicBlock* pred)
{
	this->pred.push_back(pred);
}

void SSA::BasicBlock::addSuccessor(BasicBlock* succ)
{
	this->succ.push_back(succ);
}

std::list<SSA::BasicBlock*> SSA::BasicBlock::getPredecessors()
{
	return pred;
}

std::list<SSA::BasicBlock*> SSA::BasicBlock::getSuccessors()
{
	return succ;
}

bool SSA::BasicBlock::isLoopHeader() const
{
	return loopHeader;
}
