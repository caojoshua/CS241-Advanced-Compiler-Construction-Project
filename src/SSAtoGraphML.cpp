/*
 * SSAtoGraphML.cpp
 * Author: Joshua Cao
 */

#include "SSAtoGraphML.h"


void GraphML::writeEdge(std::ofstream& f, std::map<SSA::BasicBlock*, std::string>& BBtoNodeId,
				SSA::BasicBlock* from, SSA::BasicBlock* to, int& edgeId)
{
	if (from && to)
	{
		std::string str = std::string(EDGE);
		str.replace(str.find("{id}"), 4, "e" + std::to_string(edgeId));
		str.replace(str.find("{from}"), 6, BBtoNodeId[from]);
		str.replace(str.find("{to}"), 4, BBtoNodeId[to]);
		++edgeId;
		f << str;
	}
}

void GraphML::writeFunc(std::ofstream& f, SSA::Func* func)
{
	std::string funcName = func->getName();
	std::map<SSA::BasicBlock*, std::string> BBtoNodeId;
	int bbId = 0;
	f << funcHeaderA << funcName << funcHeaderB;
	std::list<SSA::BasicBlock*> BBs = func->getBBs();
	for (SSA::BasicBlock* bb : BBs)
	{
		BBtoNodeId[bb] = funcName + std::to_string(bbId);
		f << BBHeaderA << funcName << bbId << BBHeaderB;
		for (SSA::Instruction* instruction : bb->getInstructions())
		{
			f << std::endl << instruction->toStr();
		}
		++bbId;
		f << BBFooter;
	}
	int edgeId = 0;
	for (SSA::BasicBlock* bb : BBs)
	{
		writeEdge(f, BBtoNodeId, bb, bb->getLeft(), edgeId);
		writeEdge(f, BBtoNodeId, bb, bb->getRight(), edgeId);
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


