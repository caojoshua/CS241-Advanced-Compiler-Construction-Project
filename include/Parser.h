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

	// stack of id to value mappings, where index 0 is top of stack
	std::list<std::unordered_map<std::string, SSA::Operand*>> varMapStack;
	std::list<std::unordered_map<std::string, Array>> arrayMapStack;
	int stackOffset;

	// global variables to keep track of where to emit
	SSA::Program IR;
	SSA::Func* func;
	SSA::BasicBlock* currBB;
	SSA::BasicBlock* joinBB;
	std::unordered_map<std::string, SSA::Instruction*> joinPhiList;
//	std::unordered_map<std::string, Array> joinArrayMap;

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
	SSA::Operand* compute(Opcode opcode, SSA::Operand* x, SSA::Operand* y);
	void mustParse(LexAnalysis::Token tk);
	void err();

	// var mapping
	// TODO: everything array
	void pushMap();
	void pushMap(std::unordered_map<std::string, SSA::Operand*> varMap,
			std::unordered_map<std::string, Array> arrayMap);
	void popMap();
	void assignVarValue(std::string id, SSA::Operand* value);
	void assignArrayValue(std::string id, SSA::Operand* value, int offset);
	SSA::Operand* getVarValue(std::string id);
	SSA::Operand* getArrayValue(std::string id, int offset);
	void insertPhisIntoCurrBB();

	// IR generating
	void emitFunc();
	void emitBB(SSA::BasicBlock* bb);
	void emit(SSA::BasicBlock* bb, SSA::Instruction* ins);
public:
	Parser(char const* s);
	SSA::Program& parse();
};
