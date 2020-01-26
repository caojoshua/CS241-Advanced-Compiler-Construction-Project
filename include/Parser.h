/*
 * Parser.h
 * Author: Joshua Cao
 */

#include "Scanner.h"
#include "SSA.h"
#include <string>
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
	enum Op {add, sub, mul, div};
	LexAnalysis::Scanner scan;
	// map src variables to SSA values
	std::unordered_map<std::string, SSA::Instruction*> varMap;
	int valCount;
	// grammar parsing
	void function();
	void declarationList();
	void varDeclaration();
	void arrayDeclartion();
	void functionBody();
	void statementList();
	void statement();
	Result returnStatement();
	Result callStatement();
	void whileLoop();
	void ifStatement();
	Result conditional();
	void assignment();
	void varDeclareList();
	Result expression();
	Result term();
	Result factor();
	Result value();
	Result lvalue();
	// constant folding, might remove this and do folding in IR
	void compute(Op op, Result& x, Result& y);
	void mustParse(LexAnalysis::Token tk);
	void err();
	//IR generating
	void assignId(std::string s);
public:
	Parser(char const* s);
	void parse();
};
