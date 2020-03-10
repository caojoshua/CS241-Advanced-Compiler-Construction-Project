/*
 * SSA.h
 * Author: joshua
 */

#ifndef SSA_H
#define SSA_H

#include <string>
#include <list>
#include <iostream>
#include <map>

namespace SSA
{

	enum Opcode {add, sub, mul, div, cmp, adda, load, store, move, phi, end,
		bra, bne, beq, ble, blt, bge, bgt, read, write, writeNL, call};

	// forward declarations
	class Instruction;
	class BasicBlock;

	class Operand
	{
	public:
		struct FunctionCall
		{
			std::string funcName;
			std::list<Operand*> args;
		};

		enum Type {val, phi, call, constant};
		Operand() {}
		virtual ~Operand() {}
		virtual Operand* clone() = 0;
		virtual Type getType() = 0;
		virtual bool equals(Operand* other);
		virtual std::string toStr() = 0;

		virtual Instruction* getInstruction();
		virtual FunctionCall* getFunctionCall() const;
		virtual std::string getVarName() const;
		virtual std::map<BasicBlock*, Operand*> getPhiArgs() const;
		virtual void addPhiArg(BasicBlock* b, Operand* o) {}
		virtual int getConst();
	};

	class ValOperand : public Operand
	{
	private:
		Instruction* ins;
	public:
		ValOperand(Instruction* ins) : ins(ins) {}
		virtual Operand* clone();
		Type getType();
		std::string toStr();
		Instruction* getInstruction();
	};

	class CallOperand : public Operand
	{
	private:
		FunctionCall* f;
	public:
		CallOperand(FunctionCall* f) : f(f) {}
		CallOperand(std::string funcName, std::list<Operand*> args);
		virtual Operand* clone();
		Type getType();
		std::string getFuncName() const;
		std::list<Operand*> getCallArgs() const;
		void setCallArgs(std::list<Operand*> args);
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

	class Instruction
	{
	private:
		static int idCount;
		int id;
		Opcode op;
		Operand* x;
		Operand* y;
	public:
		Instruction(Opcode op) : op(op), x(nullptr), y(nullptr), id(idCount++) {};
		Instruction(Opcode op, Operand* x) : op(op), x(x), y(nullptr), id(idCount++) {};
		Instruction(Opcode op, Operand* x, Operand* y) : op(op), x(x), y(y), id(idCount++) {};
		Instruction(const Instruction &other) : Instruction(other.op, other.x, other.y) {}
		virtual ~Instruction();
		virtual bool equals(Instruction* other);
		int const getId();
		Opcode const getOpcode();
		Operand* const getOperand1();
		Operand* const getOperand2();
		void setOperand1(Operand* o);
		void setOperand2(Operand* o);
	    virtual std::string toStr();
	    void static resetId();
	};

	class BasicBlock
	{
	private:
		std::list<Instruction*> code;
		std::list<BasicBlock*> pred;
		std::list<BasicBlock*> succ;
	public:
		BasicBlock() {}
		~BasicBlock();
		void emit(Instruction* ins);
		void emit(SSA::ValOperand*);
		void emit(std::list<Instruction*> ins);
		void emitFront(Instruction* ins);
		std::list<Instruction*>& getInstructions();
		void addPredecessor(BasicBlock* pred);
		void addSuccessor(BasicBlock* succ);
		std::list<BasicBlock*> getPredecessors();
		std::list<BasicBlock*> getSuccessors();
	};

	class Func
	{
	private:
		std::string name;
		std::list<BasicBlock*> BBs;
	public:
		Func(std::string name) : name(name) {}
		~Func();
		void emit(BasicBlock* bb);
		std::string getName();
		std::list<BasicBlock*> getBBs();
	};

	class Program
	{
	private:
		Func* mainFunc;
		std::list<Func*> funcs;
	public:
		Program() : mainFunc(nullptr) {}
		~Program();
		void emitMain(Func* f);
		void emit(Func* f);
		Func* const getMain();
		Func* const getFunc(std::string name);
		std::list<Func*>& getFuncs();
	};

	std::string opToStr(Opcode op);
};

#endif
