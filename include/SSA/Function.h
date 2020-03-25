/*
 * Function.h
 * Author: Joshua Cao
 */

#ifndef INCLUDE_SSA_FUNCTION_H_
#define INCLUDE_SSA_FUNCTION_H_

#include <string>
#include <list>
#include <unordered_set>
#include "BasicBlock.h"

namespace SSA
{

class Operand;
class Instruction;
class Function;
class Module;

class Function
	{
	private:
		std::string name;
		std::list<BasicBlock*> BBs;
		bool isVoidReturn;
		// keep track of locals offset for spilling later
		// optimally keep track of some sort of mapping for better optimizations
		int localVariableOffset;
		Module* parent;
	public:
		Function(Module* module, std::string name)
			: name(name), isVoidReturn(true), localVariableOffset(0), parent(module) {}
		Function(Module* module, std::string name, bool isVoid)
					: name(name), isVoidReturn(isVoid), localVariableOffset(0), parent(module) {}
		~Function();
		void emit(BasicBlock* bb);
		std::string getName();
		std::list<BasicBlock*> getBBs();
		Module* getParent() const;
		bool isVoid() const;
		int getLocalVariableOffset() const;
		void setIsVoid(bool isVoid);
		void setLocalVariableOffset(int i);
		int resetLineIds();
		void resetRegs();
	};

}

#endif /* INCLUDE_SSA_FUNCTION_H_ */
