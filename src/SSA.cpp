/*
 * SSA.cpp
 * Author: joshua
 */

#include "SSA.h"

SSA::Instruction* SSA::Op::getInstruction()
{
	return nullptr;
}

int SSA::Op::getMemLocation()
{
	return -1;
}

int SSA::Op::getConst()
{
	return -1;
}

SSA::Op::Type SSA::ValOp::getType()
{
	return Op::val;
}

SSA::Instruction* SSA::ValOp::getInstruction()
{
	return ins;
}

SSA::Op::Type SSA::MemAccessOp::getType()
{
	return Op::memAccess;
}

int SSA::MemAccessOp::getMemLocation()
{
	return memLocation;
}

SSA::Op::Type SSA::ConstOp::getType()
{
	return Op::constant;
}

int SSA::ConstOp::getConst()
{
	return constVal;
}


SSA::Instruction::Instruction(SSA::Op* op1, SSA::Op* op2) : op1(op1), op2(op2)
{
}

SSA::Instruction::~Instruction()
{
	delete op1;
	delete op2;
}

SSA::Op* SSA::Instruction::getOp1()
{
	return op1;
}

SSA::Op* SSA::Instruction::getOp2()
{
	return op2;
}

SSA::BasicBlock::~BasicBlock()
{
	for (Instruction* i : code)
	{
		delete i;
	}
}

void SSA::BasicBlock::addInstruction(Instruction *ins)
{
	code.push_back(ins);
}

void SSA::BasicBlock::addInstructions(std::list<Instruction*> ins)
{
	for (Instruction* i : ins)
	{
		addInstruction(i);
	}
}

std::list<SSA::Instruction*> SSA::BasicBlock::getCode()
{
	return code;
}

SSA::Func::~Func()
{
	for (BasicBlock* bb : BBs)
	{
		delete bb;
	}
}

void SSA::Func::addBasicBlock(BasicBlock *bb)
{
	BBs.push_back(bb);
}

std::list<SSA::BasicBlock*> SSA::Func::getBBs()
{
	return BBs;
}
