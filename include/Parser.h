/*
 * Parser.h
 * Author: Joshua Cao
 */

#include "Scanner.h"

class Result
{
public:
	enum Kind {const_kind, var, reg};
	Result() : kind(const_kind), val(0) {}
	Result(Kind k, int v)
		: kind(k), val(v) {}
	Kind kind;
	int val; // value if const, addr if var, regno if reg
};

class Parser
{
private:
	LexAnalysis::Scanner scan;
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
	void mustParse(LexAnalysis::Token tk);
	void err();
public:
	Parser(char const* s);
	void parse();
};
