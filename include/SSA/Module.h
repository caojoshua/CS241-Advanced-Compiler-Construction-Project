/*
 * Module.h
 * Author: Joshua Cao
 */

#ifndef INCLUDE_SSA_MODULE_H_
#define INCLUDE_SSA_MODULE_H_

#include <list>
#include <unordered_set>
#include <string>

namespace SSA
{

class Operand;
class Instruction;
class BasicBlock;
class Function;

class Module
	{
	private:
		std::list<Function*> funcs;
		std::unordered_set<SSA::Operand*> ops;
	public:
		Module();
		~Module();
		void emit(Function* f);
		Function* const getFunc(std::string name);
		std::list<Function*>& getFuncs();
		Function* getFunction(std::string name) const;
		void addOperand(SSA::Operand* o);
		// this is SUPER expensive but this whole compilers memory management sucks
		// so this works as a bandaid by clearing out unused operands
		void cleanOperands();
	};

}

#endif /* INCLUDE_SSA_MODULE_H_ */
