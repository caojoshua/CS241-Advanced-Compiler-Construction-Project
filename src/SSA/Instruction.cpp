/*
 * Instruction.cpp
 * Author: Joshua Cao
 */

#include "Instruction.h"

#include "Operand.h"
#include "BasicBlock.h"
#include "Function.h"
#include "Module.h"
#include "SSAutils.h"

uint SSA::Instruction::idCount = 0;

void SSA::Instruction::replaceArg(Operand* oldOp, Operand* newOp, bool left)
{
	Operand** opPtr;
	if (left && x)
	{
		opPtr = &x;
	}
	else if (y)
	{
		opPtr = &y;
	}
	else
	{
		return;
	}
	Operand*& o = *opPtr;
	switch (o->getType())
	{
	case Operand::val:
		if (o->equals(oldOp))
		{
			o = newOp;
		}
		break;
	case Operand::call:
	case Operand::phi:
		o->replaceArg(oldOp, newOp);
		break;
	}
}

SSA::Instruction::~Instruction()
{
//	delete x;
//	delete y;
}

SSA::Instruction* SSA::Instruction::clone() const
{
	return new Instruction(*this);
}

bool SSA::Instruction::equals(Instruction* other)
{
	return op == other->op && (!x || x->equals(other->x)) && (!y || y->equals(other->y));
}

uint SSA::Instruction::getId() const
{
	return id;
}

SSA::BasicBlock* SSA::Instruction::getParent() const
{
	return parent;
}

int SSA::Instruction::getReg() const
{
	return reg;
}

bool SSA::Instruction::hasOutput() const
{
	switch (op)
	{
	case store:
	case end:
	case bra:
	case bne:
	case beq:
	case ble:
	case blt:
	case bge:
	case bgt:
	case write:
	case writeNL:
	case ret:
		return false;
	case call:
		if (x->getType() == Operand::call)
		{
			return !x->getFunctionCall()->function->isVoid();
		}
	}
	return true;
}

void SSA::Instruction::setId(uint id)
{
	this->id = id;
}

void SSA::Instruction::setParent(BasicBlock* b)
{
	parent = b;
}

void SSA::Instruction::setReg(int reg)
{
	this->reg = reg;
}

SSA::Opcode const SSA::Instruction::getOpcode()
{
	return op;
}

SSA::Operand* const SSA::Instruction::getOperand1()
{
	return x;
}

SSA::Operand* const SSA::Instruction::getOperand2()
{
	return y;
}

void SSA::Instruction::setOperand1(Operand* o)
{
	x = o;
}

void SSA::Instruction::setOperand2(Operand* o)
{
	y = o;
}

void SSA::Instruction::insertBefore(Instruction* other)
{
	parent->emitBefore(other, this);
}

void SSA::Instruction::insertAfter(Instruction* other)
{
	parent->emitAfter(other, this);
}

void SSA::Instruction::replaceArg(Operand* oldOp, Operand* newOp)
{
//	if (containsArg(oldOp))
//	{
		parent->getParent()->getParent()->addOperand(oldOp);
//	}
	replaceArg(oldOp, newOp, true);
	replaceArg(oldOp, newOp, false);
}

bool SSA::Instruction::containsArg(Operand* o) const
{
	return (x && x->containsArg(o)) || (y && y->containsArg(o));
}

void SSA::Instruction::resetId()
{
	idCount = 0;
}

std::string SSA::Instruction::toStr()
{
	std::string s = std::to_string(id) + ": " + opToStr(op);
	if (x)
	{
		s += " " + x->toStr();
	}
	if (y)
	{
		s += " " + y->toStr();
	}
	if (reg != -1)
	{
		s = "R" + std::to_string(reg) + " = { " + s + " }";
	}
	return s;
}
