/*
 * Operand.cpp
 * Author: Joshua Cao
 */

#include "Operand.h"

#include "Instruction.h"
#include "BasicBlock.h"
#include "Function.h"
#include "Module.h"

bool SSA::Operand::equals(Operand* other)
{
	if (!other)
	{
		return false;
	}
	else if (getType() == val && other->getType() == val)
	{
		// should we be checking raw pointer equality?
		// if we ever clone instructions, maybe not...
		return getInstruction() == other->getInstruction();
	}
	else if (getType() == call && other->getType() == call)
	{
		return getFunctionCall() == other->getFunctionCall();
	}
	else if (getType() == phi && other->getType() == phi)
	{
		return (getVarName() == other->getVarName())
				&& (getPhiArgs() == other->getPhiArgs());
	}
	else if (getType() == constant && other->getType() == constant)
	{
		return getConst() == other->getConst();
	}
	else if (getType() == globalReg && other->getType() == globalReg)
	{
		return true;
	}
	return false;
}

SSA::Instruction* SSA::Operand::getInstruction()
{
	return nullptr;
}

SSA::Operand::FunctionCall* SSA::Operand::getFunctionCall() const
{
	return nullptr;
}

std::list<SSA::Operand*> SSA::Operand::getArgs() const
{
	return std::list<SSA::Operand*>();
}

bool SSA::Operand::containsArg(SSA::Operand* o)
{
	return false;
}

std::string SSA::Operand::getVarName() const
{
	return "";
}

SSA::Operand* SSA::Operand::getPhiArg(BasicBlock* b) const
{
	return nullptr;
}

std::map<SSA::BasicBlock*, SSA::Operand*> SSA::Operand::getPhiArgs() const
{
	return std::map<SSA::BasicBlock*, SSA::Operand*>();
}

int SSA::Operand::getConst()
{
	return 0;
}

SSA::Operand* SSA::ValOperand::clone()
{
	return new ValOperand(ins);
}

SSA::Operand::Type SSA::ValOperand::getType()
{
	return val;
}

SSA::Instruction* SSA::ValOperand::getInstruction()
{
	return ins;
}

void SSA::ValOperand::replaceArg(Operand* oldOp, Operand* newOp)
{
	if (equals(oldOp))
	{
		ins = newOp->getInstruction();
	}
}

bool SSA::ValOperand::containsArg(Operand* o)
{
	return equals(o);
}

std::string SSA::ValOperand::toStr()
{
	return "(" + std::to_string(ins->getId()) + ")";
}

SSA::CallOperand::CallOperand(Function* function, std::list<Operand*> args) : functionCall(new FunctionCall())
{
	functionCall->function = function;
	functionCall->args = args;
}

SSA::CallOperand::~CallOperand()
{
	delete functionCall;
}

SSA::Operand* SSA::CallOperand::clone()
{
	return new CallOperand(functionCall);
}

SSA::Operand::Type SSA::CallOperand::getType()
{
	return call;
}

SSA::Operand::FunctionCall* SSA::CallOperand::getFunctionCall() const
{
	return functionCall;
}

std::string SSA::CallOperand::getFuncName() const
{
	return functionCall->function->getName();
}

std::list<SSA::Operand*> SSA::CallOperand::getArgs() const
{
	return functionCall->args;
}

void SSA::CallOperand::replaceArg(Operand* oldOp, Operand* newOp)
{
	for (SSA::Operand*& o : functionCall->args)
	{
		if (o->equals(oldOp))
		{
			o = newOp;
		}
	}
}

bool SSA::CallOperand::containsArg(Operand* o)
{
	for (Operand* arg : functionCall->args)
	{
		if (arg->equals(o))
		{
			return true;
		}
	}
	return false;
}

std::string SSA::CallOperand::toStr()
{
	std::string s = functionCall->function->getName() + "(";
	for (Operand* o : functionCall->args)
	{
		s += o->toStr() + ' ';
	}
	return s + ")";
}

SSA::PhiOperand::PhiOperand(std::string varName, BasicBlock* b, Operand* o) : varName(varName)
{
	addPhiArg(b, o);
}

SSA::Operand* SSA::PhiOperand::clone()
{
	PhiOperand* phi = new PhiOperand(varName);
	for (std::pair<BasicBlock*, Operand*> arg : args)
	{
		phi->addPhiArg(arg.first, arg.second);
	}
	return phi;
}

SSA::Operand::Type SSA::PhiOperand::getType()
{
	return phi;
}

std::string SSA::PhiOperand::getVarName() const
{
	return varName;
}

SSA::Operand* SSA::PhiOperand::getPhiArg(BasicBlock* b) const
{
	if (args.find(b) == args.cend())
	{
		return nullptr;
	}
	return args.at(b);
}

std::list<SSA::Operand*> SSA::PhiOperand::getArgs() const
{
	std::list<Operand*> l;
	for (std::pair<BasicBlock*, Operand*> pair : args)
	{
		l.push_back(pair.second);
	}
	return l;
}

void SSA::PhiOperand::replaceArg(Operand* oldOp, Operand* newOp)
{
	for (std::pair<BasicBlock*, Operand*> pair : args)
	{
		if (pair.second->equals(oldOp))
		{
			args[pair.first] = newOp;
		}
	}
}

bool SSA::PhiOperand::containsArg(Operand* o)
{
	for (std::pair<BasicBlock*, Operand*> pair : args)
	{
		if (pair.second->equals(o))
		{
			return true;
		}
	}
	return false;
}

std::map<SSA::BasicBlock*, SSA::Operand*> SSA::PhiOperand::getPhiArgs() const
{
	return args;
}

void SSA::PhiOperand::addPhiArg(BasicBlock* b, Operand* o)
{
	args[b] = o;
}

std::string SSA::PhiOperand::toStr()
{
	std::string s = "";
	for (std::pair<BasicBlock*, Operand*> pair : args)
	{
		s += pair.second->toStr() + " ";
	}
	return s + "";
}

SSA::Operand* SSA::ConstOperand::clone()
{
	return new ConstOperand(constVal);
}

SSA::Operand::Type SSA::ConstOperand::getType()
{
	return constant;
}

int SSA::ConstOperand::getConst()
{
	return constVal;
}

std::string SSA::ConstOperand::toStr()
{
	return "#" + std::to_string(constVal);
}

SSA::Operand* SSA::GlobalRegOperand::clone()
{
	return new GlobalRegOperand;
}

SSA::Operand::Type SSA::GlobalRegOperand::getType()
{
	return globalReg;
}

std::string SSA::GlobalRegOperand::toStr()
{
	return "GLOBAL REG";
}




