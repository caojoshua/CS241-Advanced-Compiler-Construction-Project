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
	class Func;

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
		virtual int getConst();
		virtual std::list<Operand*> getArgs() const;
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
		FunctionCall* f;
	public:
		CallOperand(FunctionCall* f) : f(f) {}
		CallOperand(std::string funcName, std::list<Operand*> args);
		virtual Operand* clone();
		Type getType();
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

	class Instruction
	{
	private:
		static uint idCount;
		uint id;
		BasicBlock* parentBB;
		int reg;
		Opcode op;
		Operand* x;
		Operand* y;
	public:
	Instruction(Opcode op) : Instruction(op, nullptr, nullptr) {};
		Instruction(Opcode op, Operand* x) : Instruction(op, x, nullptr) {};
		Instruction(Opcode op, Operand* x, Operand* y)
					: op(op), parentBB(nullptr), x(x), y(y), id(idCount++), reg(-1) {};
		Instruction(const Instruction &other)
			: op(other.op), x(other.x), y(other.y), parentBB(other.parentBB),
			  reg(other.reg), id(other.id) {}
		virtual ~Instruction();
		Instruction* clone() const;
		virtual bool equals(Instruction* other);
		uint getId() const;
		BasicBlock* getParentBB() const;
		int getReg() const;
		void setId(uint id);
		void setParentBB(BasicBlock* b);
		void setReg(int reg);
		Opcode const getOpcode();
		Operand* const getOperand1();
		Operand* const getOperand2();
		void setOperand1(Operand* o);
		void setOperand2(Operand* o);
		void insertBefore(SSA::Instruction* other);
		void insertAfter(SSA::Instruction* other);
		void replaceArg(Operand* oldOp, Operand* newOp);
		bool containsArg(Operand* o) const;
	    virtual std::string toStr();
	    void static resetId();
	};

	class BasicBlock
	{
	private:
		Func* parentFunction;
		std::list<Instruction*> instructions;
		std::list<BasicBlock*> pred;
		std::list<BasicBlock*> succ;
		std::list<Instruction*>::iterator getInstructionIter(Instruction* i);
		bool loopHeader;
	public:
		BasicBlock() : parentFunction(nullptr), loopHeader(false) {}
		BasicBlock(bool loopHeader) : parentFunction(nullptr), loopHeader(loopHeader) {}
		~BasicBlock();
		Func* getParentFunction() const;
		void setParentFunction(Func* f);
		void emit(Instruction* ins);
		void emit(SSA::ValOperand*);
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

	class Func
	{
	private:
		std::string name;
		std::list<BasicBlock*> BBs;
		// keep track of locals offset for spilling later
		// optimally keep track of some sort of mapping for better optimizations
		int localVariableOffset;
	public:
		Func(std::string name) : name(name), localVariableOffset(0) {}
		~Func();
		void emit(BasicBlock* bb);
		std::string getName();
		std::list<BasicBlock*> getBBs();
		int getLocalVariableOffset() const;
		void setLocalVariableOffset(int i);
	};

	class IntermediateRepresentation
	{
	private:
		Func* mainFunc;
		std::list<Func*> funcs;
	public:
		IntermediateRepresentation() : mainFunc(nullptr) {}
		~IntermediateRepresentation();
		void emitMain(Func* f);
		void emit(Func* f);
		Func* const getMain();
		Func* const getFunc(std::string name);
		std::list<Func*>& getFuncs();
	};

	std::string opToStr(Opcode op);
};

#endif
