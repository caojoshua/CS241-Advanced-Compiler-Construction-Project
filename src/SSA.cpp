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
	else if (getType() == stackPointer && other->getType() == stackPointer)
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
		s += pair.second->toStr() +
				", " + std::to_string(pair.first->getInstructions().size()) + " ";
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

SSA::Operand* SSA::StackPointerOperand::clone()
{
	return new StackPointerOperand;
}

SSA::Operand::Type SSA::StackPointerOperand::getType()
{
	return stackPointer;
}

std::string SSA::StackPointerOperand::toStr()
{
	return "SP";
}

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
	case SSA::Operand::val:
		if (o->equals(oldOp))
		{
			o = newOp;
		}
		break;
	case SSA::Operand::call:
	case SSA::Operand::phi:
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

void SSA::BasicBlock::emit(SSA::ValOperand* op)
{
	emit(op->getInstruction());
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

//void SSA::Function::addOperand(Operand* o)
//{
//	if (o)
//	{
//		ops.insert(o);
//	}
//}

SSA::Module::Module()
{
	funcs.push_back(new Function(this, "InputNum", false));
	funcs.push_back(new Function(this, "OutputNum", true));
	funcs.push_back(new Function(this, "OutputNewLine", true));
}

SSA::Module::~Module()
{
	for (Function* f : funcs)
	{
		delete f;
	}
	// temporary solution to delete operands, since freeing operands
	// from instructions cause double frees
	for (SSA::Operand* o : ops)
	{
		delete o;
	}
}

void SSA::Module::emit(Function* f)
{
	funcs.push_back(f);
}

SSA::Function* SSA::Module::getFunction(std::string name) const
{
	for (Function* f : funcs)
	{
		if (f->getName() == name)
		{
			return f;
		}
	}
	return nullptr;
}

void SSA::Module::addOperand(Operand* o)
{
	if (o)
	{
		ops.insert(o);
	}
}

void SSA::Module::cleanOperands()
{
	std::unordered_set<SSA::Operand*> deleteOps;
	for (SSA::Operand* o : ops)
	{
		bool found = false;
		for (Function* f : funcs)
		{
			for (BasicBlock* b : f->getBBs())
			{
				for (SSA::Instruction* i : b->getInstructions())
				{
					bool inUse = false;
					if (i->getOperand1())
					{
						if (i->getOperand1() == o)
						{
							inUse = true;
						}
						for (SSA::Operand* arg : i->getOperand1()->getArgs())
						{
							if (arg == o)
							{
								inUse = true;
							}
						}
					}
					if (i->getOperand2())
					{
						if (i->getOperand2() == o)
						{
							inUse = true;
						}
						for (SSA::Operand* arg : i->getOperand2()->getArgs())
						{
							if (arg == o)
							{
								inUse = true;
							}
						}
					}

					if (!inUse)
					{
//						printf("found: %s\n", o->toStr().c_str());
						found = true;
					}
				}
			}
		}
		if (!found)
		{
//			printf("%s\n", o->toStr().c_str());
			deleteOps.insert(o);
//			ops.erase(o);
//			delete o;
		}
	}

	for (SSA::Operand* o : deleteOps)
	{
		ops.erase(o);
		delete(o);
	}
}

std::list<SSA::Function*>& SSA::Module::getFuncs()
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
	case ret:		return "ret";
	case pop:		return "pop";
	case constant:	return "constant";
	}
	return "";
}

std::string SSA::ValOperand::toStr()
{
	return "(" + std::to_string(ins->getId()) + ")";
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
		s = "R" + std::to_string(reg) + " = { " + s + " }";
	}
	return s;
}


