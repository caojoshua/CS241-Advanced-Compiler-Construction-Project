/*
 * Parser.h
 * Author: Joshua Cao
 */

#include "Scanner.h"
#include "SSA.h"
#include <string>
#include <vector>
#include <unordered_map>

class Result
{
public:
	enum Kind {constKind, var, reg};
	Kind kind;
	int val; // value if const, addr if var, regno if reg
	Result() : kind(constKind), val(0) {}
	Result(Kind k, int v)
		: kind(k), val(v) {}
};

class Parser
{
private:
	enum Opcode {add, sub, mul, div};
	class Array
	{
	private:
		static int totalOffset;
		int offset;
		std::vector<SSA::Operand*> vals;
	public:
		Array() : Array(0) {}
		Array(int length);
		int getOffset();
		void assign(int index, SSA::Operand* operand);
		SSA::Operand* getOperand(int index);
	};

	LexAnalysis::Scanner scan;
	// map source variables to SSA values
	std::unordered_map<std::string, SSA::Operand*> varMap;
	std::unordered_map<std::string, Array> arrayMap;
	int stackOffset;
	// variables to keep track of where to emit
	SSA::Program IR;
	SSA::Func* func;
	SSA::BasicBlock* currBB;
	SSA::BasicBlock* joinBB;

	// grammar parsing
	void function();
	void declarationList();
	void varDeclaration();
	void arrayDeclartion();
	void functionBody();
	void statementList();
	void statement();
	void returnStatement();
	SSA::Operand* callStatement();
	void whileLoop();
	void ifStatement();
	void conditional();
	void assignment();
	void varDeclareList();
	SSA::Operand* expression();
	SSA::Operand* term();
	SSA::Operand* factor();
	SSA::Operand* value();
	SSA::Operand* lvalue();
	// constant folding
	SSA::Operand* compute(Opcode opcode, SSA::Operand* x, SSA::Operand* y);
	void mustParse(LexAnalysis::Token tk);
	void err();

	// IR generating
	void emitFunc();
	// need to pass bb because not always emitting in order
	void emitBB(SSA::BasicBlock* bb);
	void emit(SSA::BasicBlock* bb, SSA::Instruction* ins);
	SSA::Operand* getVarValue(std::string id);
	SSA::Operand* getArrayValue(std::string id, int offset);
public:
	Parser(char const* s);
	SSA::Program& parse();
};
