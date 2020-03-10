/*
 * Parser.h
 * Author: Joshua Cao
 */

#include "Scanner.h"
#include "SSA.h"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

const static int INT_SIZE = 4;

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
		std::vector<int> dims;
	public:
		Array() : offset(totalOffset) {}
		Array(std::vector<int> dims);
		Array& operator=(const Array other);
		static void resetTotalOffset();
		int getOffset();
		std::vector<int> getDims() const;
	};

	LexAnalysis::Scanner scan;

	// stack of id to value mappings, where index 0 is top of stack
	std::list<std::unordered_map<std::string, SSA::Operand*>> varMapStack;
	std::unordered_map<std::string, Array> arrayMap;

	// global variables to keep track of where to emit
	SSA::Program IR;
	SSA::Func* func;
	SSA::BasicBlock* currBB;
	SSA::BasicBlock* joinBB;

	// use chain stack to keep track of which instructions to propaget phis into
	std::list<std::unordered_map<SSA::Operand*, std::list<SSA::Instruction*>>> useChain;

	// keep track of instructions for CSE
	std::list<std::map<SSA::Opcode, std::list<SSA::Instruction*>>> cseStack;

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
	SSA::Operand* arrayIndexReference();

	// parsing helpers
	SSA::Operand* compute(Opcode opcode, SSA::Operand* x, SSA::Operand* y);
	void mustParse(LexAnalysis::Token tk);
	void err();

	// basic block linking
	void linkBB(SSA::BasicBlock* pred, SSA::BasicBlock* succ);

	// var mapping
	void pushVarMap();
	void pushVarMap(std::unordered_map<std::string, SSA::Operand*>);
	void popVarMap();
	void assignVarValue(std::string id, SSA::Operand* value);
	SSA::Operand* getVarValue(std::string id, bool fromExpression = true);

	// phi helper functions
	void pushUseChain();
	void popUseChain();
	void insertIntoUseChain(SSA::Operand* operand, SSA::Instruction* ins);
	void replaceOldOperandWithPhi(SSA::Operand* oldOperand, SSA::Operand* newOperand,
			SSA::Instruction* ins, bool left);
	void insertPhis(SSA::BasicBlock* from, SSA::BasicBlock* to);
	void commitPhis(SSA::BasicBlock* b, bool loop = false);

	// CSE
	void pushCSEmap();
	void popCSEmap();
	SSA::Instruction* cseCheck(SSA::Instruction* ins);

	// IR emitting
	void emitFunc();
	void emitBB(SSA::BasicBlock* bb);
	void emit(SSA::BasicBlock* bb, SSA::Instruction* ins);
public:
	Parser(char const* s);
	SSA::Program& parse();
};
