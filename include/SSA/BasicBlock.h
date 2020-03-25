/*
 * BasicBlock.h
 * Author: Joshua Cao
 */

#ifndef INCLUDE_SSA_BASICBLOCK_H_
#define INCLUDE_SSA_BASICBLOCK_H_

#include <list>
#include <unordered_set>

namespace SSA
{

class Operand;
class Instruction;
class Function;
class Module;

class BasicBlock
	{
	private:
		Function* parent;
		std::list<Instruction*> instructions;
		std::list<BasicBlock*> pred;
		std::list<BasicBlock*> succ;
		std::list<Instruction*>::iterator getInstructionIter(Instruction* i);
		bool loopHeader;
	public:
		BasicBlock() : parent(nullptr), loopHeader(false) {}
		BasicBlock(bool loopHeader) : parent(nullptr), loopHeader(loopHeader) {}
		~BasicBlock();
		Function* getParent() const;
		void setParent(Function* f);
		void emit(Instruction* ins);
		void emit(std::list<Instruction*> ins);
		void emitFront(Instruction* ins);
		void emitBefore(Instruction* x, Instruction* y);
		void emitAfter(Instruction* x, Instruction* y);
		std::list<Instruction*>& getInstructions();
		void addPredecessor(BasicBlock* pred);
		void addSuccessor(BasicBlock* succ);
		std::list<BasicBlock*> getPredecessors();
		std::list<BasicBlock*> getSuccessors();
		bool isLoopHeader() const;
	};

}

#endif /* INCLUDE_SSA_BASICBLOCK_H_ */
