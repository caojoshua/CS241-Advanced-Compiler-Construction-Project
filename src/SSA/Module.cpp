/*
 * Module.cpp
 * Author: Joshua Cao
 */

#include "Module.h"

#include "Operand.h"
#include "Instruction.h"
#include "BasicBlock.h"
#include "Function.h"

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


