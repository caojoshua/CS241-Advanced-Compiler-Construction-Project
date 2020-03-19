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
		bra, bne, beq, ble, blt, bge, bgt, read, write, writeNL, call, ret,
		pop, constant};

	// forward declarations
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

		enum Type {val, phi, call, constant, stackPointer};
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

	class StackPointerOperand : public Operand
	{
	public:
		StackPointerOperand() {}
		virtual Operand* clone();
		Operand::Type getType();
		std::string toStr();
	};

	class Instruction
	{
	private:
		static uint idCount;
		uint id;
		BasicBlock* parent;
		int reg;
		Opcode op;
		Operand* x;
		Operand* y;
		void replaceArg(Operand* oldOp, Operand* newOp, bool left);
	public:
	Instruction(Opcode op) : Instruction(op, nullptr, nullptr) {};
		Instruction(Opcode op, Operand* x) : Instruction(op, x, nullptr) {};
		Instruction(Opcode op, Operand* x, Operand* y)
					: op(op), parent(nullptr), x(x), y(y), id(idCount++), reg(-1) {};
		Instruction(const Instruction &other)
			: op(other.op), x(other.x), y(other.y), parent(other.parent),
			  reg(other.reg), id(other.id) {}
		virtual ~Instruction();
		Instruction* clone() const;
		virtual bool equals(Instruction* other);
		uint getId() const;
		BasicBlock* getParent() const;
		int getReg() const;
		bool hasOutput() const;
		void setId(uint id);
		void setParent(BasicBlock* b);
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
		Module* getParent() const;
		bool isVoid() const;
		std::list<BasicBlock*> getBBs();
		int getLocalVariableOffset() const;
		void setIsVoid(bool isVoid);
		void setLocalVariableOffset(int i);
		int resetLineIds();
		void resetRegs();
	};

	class Module
	{
	private:
		std::list<Function*> funcs;
	public:
		Module();
		~Module();
		void emit(Function* f);
		Function* const getFunc(std::string name);
		std::list<Function*>& getFuncs();
		Function* getFunction(std::string name) const;
	};

	std::string opToStr(Opcode op);
};

#endif
