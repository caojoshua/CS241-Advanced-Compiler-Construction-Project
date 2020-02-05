/*
 * SSAtoGraphML.cpp
 * Author: Joshua Cao
 */

#include "SSAtoGraphML.h"

void GraphML::writeFunc(std::ofstream& f, SSA::Func* func)
{
	std::string funcName = func->getName();
	int bbId = 0;
	f << funcHeaderA << funcName << funcHeaderB;
	for (SSA::BasicBlock* bb : func->getBBs())
	{
		f << BBHeaderA << funcName << bbId << BBHeaderB;
		for (SSA::Instruction* instruction : bb->getCode())
		{
			f << std::endl << instruction->toStr();
		}
		f << BBFooter;
	}
	f << funcFooter;
}

void GraphML::SSAtoGraphML(SSA::Program ssa, char const* s)
{
	std::string str = std::string(s);
	std::size_t found = str.find_last_of("/\\");
	std::string fname = "graphml/" + str.substr(found+1);
	std::ofstream f(fname);
	if (f)
	{
		f << header;
		writeFunc(f, ssa.getMain());
		for (SSA::Func* func : ssa.getFuncs())
		{
			writeFunc(f, func);
		}
		f << footer;
		f.close();
	}
	else
	{
		std::cout << "unable to open file " << fname << std::endl;
	}
}


