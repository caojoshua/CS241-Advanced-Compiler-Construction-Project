/*
 * SSA.h
 * Author: joshua
 */

#ifndef SSA_H
#define SSA_H

#include <string>
#include <list>

namespace SSA
{

	class Instruction;

	class Op
	{
	protected:
		enum Type {val, memAccess, constant};
	public:
		Op() {}
		virtual ~Op() {}
		virtual Type getType() = 0;
		virtual Instruction* getInstruction();
		virtual int getMemLocation();
		virtual int getConst();
	};

	class ValOp : Op
	{
	private:
		Instruction* ins;
	public:
		ValOp(Instruction* ins) : ins(ins) {}
		Op::Type getType();
		Instruction* getInstruction();
	};

	class MemAccessOp : Op
	{
		private:
			int memLocation;
		public:
			MemAccessOp(int memLocation) : memLocation(memLocation) {}
			Op::Type getType();
			int getMemLocation();
	};

	class ConstOp : Op
	{
		private:
			int constVal;
		public:
			ConstOp(int constVal) : constVal(constVal) {}
			Op::Type getType();
			int getConst();
	};

	class Instruction
	{
	private:
		Op* op1;
		Op* op2;
	public:
		Instruction(Op* Op1, Op* Op2);
		~Instruction();
		Op* getOp1();
		Op* getOp2();
	};

	class BasicBlock
	{
	private:
		std::list<Instruction*> code;
	public:
		~BasicBlock();
		void addInstruction(Instruction* ins);
		void addInstructions(std::list<Instruction*> ins);
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
		void addBasicBlock(BasicBlock* bb);
		std::list<BasicBlock*> getBBs();
	};

};

#endif
