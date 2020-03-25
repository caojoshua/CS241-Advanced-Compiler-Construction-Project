/*
 * Instruction.h
 * Author: Joshua Cao
 */

#ifndef INCLUDE_SSA_INSTRUCTION_H_
#define INCLUDE_SSA_INSTRUCTION_H_

#include <string>
#include "Operand.h"

namespace SSA
{

class Operand;
class BasicBlock;
class Function;
class Module;

enum Opcode : uint {add, sub, mul, div, cmp, adda, load, store, move, phi, end,
		bra, bne, beq, ble, blt, bge, bgt, read, write, writeNL, call, ret,
		pop, constant};

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

}


#endif /* INCLUDE_SSA_INSTRUCTION_H_ */
