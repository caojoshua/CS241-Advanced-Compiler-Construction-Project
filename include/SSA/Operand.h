/*
 * Operand.h
 * Author: Joshua Cao
 */

#ifndef INCLUDE_SSA_OPERAND_H_
#define INCLUDE_SSA_OPERAND_H_

#include <list>
#include <map>
#include <string>

namespace SSA
{

class Instruction;
class BasicBlock;
class Function;
class Module;

class Operand
	{
	public:
		struct FunctionCall
		{
			Function* function;
			std::list<Operand*> args;
		};

		enum Type {val, phi, call, constant, globalReg};
		Operand() {}
		virtual ~Operand() {}
		virtual Operand* clone() = 0;
		virtual Type getType() = 0;
		virtual bool equals(Operand* other);
		virtual std::string toStr() = 0;

		virtual Instruction* getInstruction();
		virtual FunctionCall* getFunctionCall() const;
		virtual int getConst();
		virtual std::list<Operand*> getArgs() const;
		// this is broken. it modifies the current operand, modifying everything
		// that points to it.
		virtual void replaceArg(SSA::Operand* oldOp, SSA::Operand* newOp) {}
		virtual bool containsArg(SSA::Operand* o);

		virtual std::string getVarName() const;
		virtual Operand* getPhiArg(BasicBlock* b) const;
		virtual std::map<BasicBlock*, Operand*> getPhiArgs() const;
		virtual void addPhiArg(BasicBlock* b, Operand* o) {}
	};

	class ValOperand : public Operand
	{
	private:
		Instruction* ins;
	public:
		ValOperand(Instruction* ins) : ins(ins) {}
		virtual Operand* clone();
		Type getType();
		Instruction* getInstruction();
		virtual void replaceArg(SSA::Operand* oldOp, SSA::Operand* newOp);
		virtual bool containsArg(SSA::Operand* o);
		std::string toStr();
	};

	class CallOperand : public Operand
	{
	private:
		FunctionCall* functionCall;
	public:
		CallOperand(FunctionCall* f) : functionCall(f) {}
		CallOperand(Function* f, std::list<Operand*> args);
		~CallOperand();
		virtual Operand* clone();
		Type getType();
		FunctionCall* getFunctionCall() const;
		std::string getFuncName() const;
		std::list<Operand*> getArgs() const;
		void replaceArg(SSA::Operand* oldOp, SSA::Operand* newOp);
		virtual bool containsArg(SSA::Operand* o);
		std::string toStr();
	};

	class PhiOperand : public Operand
	{
	private:
		std::string varName;
		std::map<BasicBlock*, Operand*> args;
	public:
		PhiOperand(std::string varName) : varName(varName) {}
		PhiOperand(std::string varName, BasicBlock* b, Operand* o);
		virtual Operand* clone();
		Type getType();
		std::string getVarName() const;
		Operand* getPhiArg(BasicBlock* b) const;
		std::list<Operand*> getArgs() const;
		void replaceArg(SSA::Operand* oldOp, SSA::Operand* newOp);
		virtual bool containsArg(SSA::Operand* o);
		std::map<BasicBlock*, Operand*> getPhiArgs() const;
		void addPhiArg(BasicBlock* b, Operand* o);
		std::string toStr();
	};

	class ConstOperand : public Operand
	{
	private:
		int constVal;
	public:
		ConstOperand(int constVal) : constVal(constVal) {}
		virtual Operand* clone();
		Operand::Type getType();
		std::string toStr();
		int getConst();
	};

	class GlobalRegOperand : public Operand
	{
	public:
		GlobalRegOperand() {}
		virtual Operand* clone();
		Operand::Type getType();
		std::string toStr();
	};

}

#endif /* INCLUDE_SSA_OPERAND_H_ */
