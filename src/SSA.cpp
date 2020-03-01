/*
 * SSA.cpp
 * Author: joshua
 */

#include "SSA.h"

SSA::Instruction* SSA::Operand::getInstruction()
{
	return nullptr;
}

int SSA::Operand::getMemLocation()
{
	return -1;
}

int SSA::Operand::getConst()
{
	return -1;
}

SSA::Operand::Type SSA::ValOperand::getType()
{
	return val;
}

SSA::Instruction* SSA::ValOperand::getInstruction()
{
	return ins;
}

SSA::Operand::Type SSA::MemAccessOperand::getType()
{
	return memAccess;
}

int SSA::MemAccessOperand::getMemLocation()
{
	return memLocation;
}

SSA::Operand::Type SSA::CallOperand::getType()
{
	return call;
}

SSA::Operand::Type SSA::ConstOperand::getType()
{
	return constant;
}

int SSA::ConstOperand::getConst()
{
	return constVal;
}

int SSA::Instruction::idCount = 0;

SSA::Instruction::~Instruction()
{
//	delete x;
//	delete y;
}

int const SSA::Instruction::getId()
{
	return id;
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

SSA::BasicBlock::BasicBlock() : left(nullptr), right(nullptr)
{
}

SSA::BasicBlock::~BasicBlock()
{
	for (Instruction* i : code)
	{
		// delete i;
	}
}

void SSA::BasicBlock::emit(Instruction *ins)
{
	code.push_back(ins);
}

void SSA::BasicBlock::emit(SSA::ValOperand* op)
{
	code.push_back(op->getInstruction());
}

void SSA::BasicBlock::emit(std::list<Instruction*> ins)
{
	for (Instruction* i : ins)
	{
		emit(i);
	}
}

std::list<SSA::Instruction*> SSA::BasicBlock::getCode()
{
	return code;
}

SSA::BasicBlock* SSA::BasicBlock::getLeft() const
{
	return left;
}

void SSA::BasicBlock::setLeft(BasicBlock *left)
{
	this->left = left;
}

SSA::BasicBlock* SSA::BasicBlock::getRight() const
{
	return right;
}

void SSA::BasicBlock::setRight(BasicBlock *right)
{
	this->right = right;
}

SSA::Func::~Func()
{
	for (BasicBlock* bb : BBs)
	{
//		delete bb;
	}
}

void SSA::Func::emit(BasicBlock *bb)
{
	BBs.push_back(bb);
}

std::string SSA::Func::getName()
{
	return name;
}

std::list<SSA::BasicBlock*> SSA::Func::getBBs()
{
	return BBs;
}

SSA::Program::~Program()
{
	for (Func* f : funcs)
	{
		//delete f;
	}
}

void SSA::Program::emitMain(Func* f)
{
	mainFunc = f;
}

void SSA::Program::emit(Func* f)
{
	funcs.push_back(f);
}

SSA::Func* const SSA::Program::getMain()
{
	return mainFunc;
}

SSA::Func* const SSA::Program::getFunc(std::string name)
{
	for (Func* f : funcs)
	{
		if (f->getName() == name)
		{
			return f;
		}
	}
	return nullptr;
}

std::list<SSA::Func*>& SSA::Program::getFuncs()
{
	return funcs;
}

std::string SSA::opToStr(Opcode op)
{
	switch(op)
	{
	case add: 		return "add";
	case sub: 		return "sub";
	case mul: 		return "mul";
	case div: 		return "div";
	case cmp: 		return "cmp";
	case adda: 		return "adda";
	case load:		return "load";
	case store:		return "store";
	case move:		return "move";
	case phi:		return "phi";
	case end:		return "end";
	case bra:		return "bra";
	case bne:		return "bne";
	case beq:		return "beq";
	case ble:		return "ble";
	case blt:		return "blt";
	case bge:		return "bge";
	case bgt:		return "bgt";
	case read:		return "read";
	case write:		return "write";
	case writeNL:	return "writeNL";
	case call:		return "call";
	}
	return "";
}

std::string SSA::ValOperand::toStr()
{
	return "(" + std::to_string(ins->getId()) + ")";
}

std::string SSA::MemAccessOperand::toStr()
{
	return "[" + std::to_string(memLocation) + "]";
}

std::string SSA::CallOperand::toStr()
{
	std::string s = funcName + "(";
	for (Operand* o : args)
	{
		s += o->toStr() + ' ';
	}
	return s + ")";
}

std::string SSA::ConstOperand::toStr()
{
	return "#" + std::to_string(constVal);
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
	return s;
}
