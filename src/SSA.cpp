/*
 * SSA.cpp
 * Author: joshua
 */

#include "SSA.h"

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
	return -1;
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

SSA::CallOperand::CallOperand(std::string funcName, std::list<Operand*> args)
{
	f = new FunctionCall();
	f->funcName = funcName;
	f->args = args;
}

SSA::Operand* SSA::CallOperand::clone()
{
	return new CallOperand(f);
}

SSA::Operand::Type SSA::CallOperand::getType()
{
	return call;
}

std::string SSA::CallOperand::getFuncName() const
{
	return f->funcName;
}

std::list<SSA::Operand*> SSA::CallOperand::getArgs() const
{
	return f->args;
}

void SSA::CallOperand::replaceArg(Operand* oldOp, Operand* newOp)
{
	for (SSA::Operand*& o : f->args)
	{
		if (o == oldOp)
		{
			o = newOp;
		}
	}
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
		if (pair.second == oldOp)
		{
			args[pair.first] = newOp;
		}
	}
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

uint SSA::Instruction::idCount = 0;

SSA::Instruction::~Instruction()
{
//	delete x;
//	delete y;
}

bool SSA::Instruction::equals(Instruction* other)
{
	return op == other->op && x->equals(other->x) && y->equals(other->y);
}

uint SSA::Instruction::getId() const
{
	return id;
}

int SSA::Instruction::getReg() const
{
	return reg;
}

void SSA::Instruction::setId(uint id)
{
	this->id = id;
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

void SSA::Instruction::resetId()
{
	idCount = 0;
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

void SSA::BasicBlock::emitFront(Instruction *ins)
{
	code.push_front(ins);
}

std::list<SSA::Instruction*>& SSA::BasicBlock::getInstructions()
{
	return code;
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

SSA::IntermediateRepresentation::~IntermediateRepresentation()
{
	for (Func* f : funcs)
	{
		//delete f;
	}
}

void SSA::IntermediateRepresentation::emitMain(Func* f)
{
	mainFunc = f;
}

void SSA::IntermediateRepresentation::emit(Func* f)
{
	funcs.push_back(f);
}

SSA::Func* const SSA::IntermediateRepresentation::getMain()
{
	return mainFunc;
}

SSA::Func* const SSA::IntermediateRepresentation::getFunc(std::string name)
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

std::list<SSA::Func*>& SSA::IntermediateRepresentation::getFuncs()
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

std::string SSA::CallOperand::toStr()
{
	std::string s = f->funcName + "(";
	for (Operand* o : f->args)
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
	if (reg != -1)
	{
		s = std::to_string(reg) + " = { " + s + " }";
	}
	return s;
}


