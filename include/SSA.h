/*
 * SSA.h
 * Author: joshua
 */

#ifndef SSA_H
#define SSA_H

#include <string>
#include <list>
#include <iostream>

namespace SSA
{

	enum Opcode {add, sub, mul, div, cmp, adda, load, store, move, phi, end,
		bra, bne, beq, ble, blt, bge, bgt, read, write, writeNL, call};

	class Instruction;

	class Operand
	{
	public:
		enum Type {val, memAccess, call, constant};
		Operand() {}
		virtual ~Operand() {}
		virtual Type getType() = 0;
		virtual std::string toStr() = 0;
		virtual Instruction* getInstruction();
		virtual int getMemLocation();
		virtual int getConst();
	};

	class ValOperand : public Operand
	{
	private:
		Instruction* ins;
	public:
		ValOperand(Instruction* ins) : ins(ins) {}
		Type getType();
		std::string toStr();
		Instruction* getInstruction();
	};

	class MemAccessOperand : public Operand
	{
		private:
			int memLocation;
		public:
		MemAccessOperand(int memLocation) : memLocation(memLocation) {}
		Type getType();
		std::string toStr();
		int getMemLocation();
	};

	class CallOperand : public Operand
	{
	private:
		std::string funcName;
		std::list<Operand*> args;
	public:
		CallOperand(std::string funcName, std::list<Operand*> args) : funcName(funcName), args(args) {}
		Type getType();
		std::string toStr();
	};

	class ConstOperand : public Operand
	{
		private:
			int constVal;
		public:
			ConstOperand(int constVal) : constVal(constVal) {}
			Operand::Type getType();
			std::string toStr();
			int getConst();
	};

	class Instruction
	{
	private:
		static int idCount;
		// each instruction has unique id
		int id;
		Opcode op;
		Operand* x;
		Operand* y;
	public:
		Instruction(Opcode op) : op(op), x(nullptr), y(nullptr), id(idCount++) {};
		Instruction(Opcode op, Operand* x) : op(op), x(x), y(nullptr), id(idCount++) {};
		Instruction(Opcode op, Operand* x, Operand* y) : op(op), x(x), y(y), id(idCount++) {};
		~Instruction();
		int const getId();
		Opcode const getOpcode();
		Operand* const getOperand1();
		Operand* const getOperand2();
	    std::string toStr();
	};

	class BasicBlock
	{
	private:
		std::list<Instruction*> code;
	public:
		~BasicBlock();
		void emit(Instruction* ins);
		void emit(SSA::ValOperand*);
		void emit(std::list<Instruction*> ins);
		std::list<Instruction*> getCode();
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
