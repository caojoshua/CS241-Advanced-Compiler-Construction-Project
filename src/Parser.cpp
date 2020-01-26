/*
 * Parser.cpp
 * Author: Joshua Cao
 */

#include "Parser.h"

Parser::Parser(char const* s) : scan(s), valCount(0)
{
	scan.next();
}

// TODO
void Parser::parse() {
	mustParse(LexAnalysis::main);
	declarationList();
	functionBody();
	mustParse(LexAnalysis::period);
}

// TODO: store function somewhere, still not sure how to handle functions
void Parser::function()
{
	mustParse(LexAnalysis::func);
	mustParse(LexAnalysis::id_tk);
	if (scan.tk == LexAnalysis::open_paren)
	{
		mustParse(LexAnalysis::open_paren);
		if (scan.tk == LexAnalysis::id_tk)
		{
			varDeclareList();
		}
		mustParse(LexAnalysis::close_paren);
	}
	mustParse(LexAnalysis::semicolon);
	declarationList();
	functionBody();
	mustParse(LexAnalysis::semicolon);
}

// TODO
void Parser::declarationList() {
	while (true)
	{
		switch(scan.tk)
		{
		case LexAnalysis::var:
			varDeclaration();
			break;
		case LexAnalysis::array:
			arrayDeclartion();
			break;
		case LexAnalysis::func:
			function();
			break;
		default:
			return;
		}
	}
}

void Parser::varDeclaration()
{
	mustParse(LexAnalysis::var);
	varDeclareList();
	mustParse(LexAnalysis::semicolon);
}

// TODO: store variables in symbol table
void Parser::varDeclareList()
{
	mustParse(LexAnalysis::id_tk);
	while (scan.tk == LexAnalysis::comma)
	{
		mustParse(LexAnalysis::comma);
		mustParse(LexAnalysis::id_tk);
	}
}

// TODO: store array in symbol table
void Parser::arrayDeclartion() {
	mustParse(LexAnalysis::array);
	do
	{
		mustParse(LexAnalysis::open_bracket);
		mustParse(LexAnalysis::num_tk);
		mustParse(LexAnalysis::close_bracket);
	} while (scan.tk == LexAnalysis::open_bracket);
	varDeclareList();
	mustParse(LexAnalysis::semicolon);
}

void Parser::functionBody()
{
	mustParse(LexAnalysis::open_curlybrace);
	statementList();
	mustParse(LexAnalysis::close_curlybrace);
}

void Parser::statementList()
{
	if (scan.tk != LexAnalysis::let
			&& scan.tk != LexAnalysis::return_tk
			&& scan.tk != LexAnalysis::call
			&& scan.tk != LexAnalysis::while_tk
			&& scan.tk != LexAnalysis::if_tk)
	{
		return;
	}
	statement();
	while (scan.tk == LexAnalysis::semicolon)
	{
		mustParse(LexAnalysis::semicolon);
		statement();
	}
}

void Parser::statement()
{
	switch(scan.tk)
	{
	case LexAnalysis::let:
		assignment();
		break;
	case LexAnalysis::return_tk:
		returnStatement();
		break;
	case LexAnalysis::call:
		callStatement();
		break;
	case LexAnalysis::while_tk:
		whileLoop();
		break;
	case LexAnalysis::if_tk:
		ifStatement();
		break;
	default:
		err();
	}
}

void Parser::whileLoop()
{
	mustParse(LexAnalysis::while_tk);
	conditional();
	mustParse(LexAnalysis::do_tk);
	statementList();
	mustParse(LexAnalysis::od);
}

void Parser::ifStatement()
{
	mustParse(LexAnalysis::if_tk);
	conditional();
	mustParse(LexAnalysis::then);
	statementList();
	if (scan.tk == LexAnalysis::else_tk)
	{
		mustParse(LexAnalysis::else_tk);
		statementList();
	}
	mustParse(LexAnalysis::fi);
}

// TODO: compute results of conditional and store in x
Result Parser::conditional()
{
	Result x, y;
	x = expression();
	switch (scan.tk)
	{
	case LexAnalysis::e:
		mustParse(LexAnalysis::e);
		y = expression();
		break;
	case LexAnalysis::ne:
		mustParse(LexAnalysis::ne);
		y = expression();
		break;
	case LexAnalysis::lt:
		mustParse(LexAnalysis::lt);
		y = expression();
		break;
	case LexAnalysis::gt:
		mustParse(LexAnalysis::gt);
		y = expression();
		break;
	case LexAnalysis::lte:
		mustParse(LexAnalysis::lte);
		y = expression();
		break;
	case LexAnalysis::gte:
		mustParse(LexAnalysis::gte);
		y = expression();
		break;
	default:
		err();
	}
	return x;
}

Result Parser::returnStatement()
{
	mustParse(LexAnalysis::return_tk);
	if (scan.tk == LexAnalysis::id_tk
			|| scan.tk == LexAnalysis::num_tk
			|| scan.tk == LexAnalysis::call)
	{
		return expression();
	}
	return Result(Result::constKind, 0);
}

void Parser::assignment()
{
	mustParse(LexAnalysis::let);
	lvalue();
	mustParse(LexAnalysis::assign);
	// TODO: associate expression result with lvalue
	expression();
}

// TODO: maybe store result in left hand, not sure how to handle functions yet
Result Parser::callStatement()
{
	mustParse(LexAnalysis::call);
	mustParse(LexAnalysis::id_tk);
	if (scan.tk == LexAnalysis::open_paren)
	{
		mustParse(LexAnalysis::open_paren);
		if (scan.tk != LexAnalysis::close_paren)
		{
			expression();
			while (scan.tk == LexAnalysis::comma)
			{
				mustParse(LexAnalysis::comma);
				expression();
			}
		}
		mustParse(LexAnalysis::close_paren);
	}
	return Result();
}

Result Parser::expression()
{
	Result x, y;
	x = term();
	while (true)
	{
		if (scan.tk == LexAnalysis::add)
		{
			mustParse(LexAnalysis::add);
			y = term();
			// TODO: compute (ADD,x,y) and store in x
		}
		else if (scan.tk == LexAnalysis::sub)
		{
			mustParse(LexAnalysis::sub);
			y = term();
			// TODO: compute (SUB,x,y) and store in x
		}
		else
		{
			break;
		}
	}
	return x;
}

Result Parser::term()
{
	Result x, y;
	x = factor();
	while (true)
	{
		if (scan.tk == LexAnalysis::mul)
		{
			mustParse(LexAnalysis::mul);
			y = term();
			// TODO: compute (MUL,x,y) and store in x
		}
		else if (scan.tk == LexAnalysis::div)
		{
			mustParse(LexAnalysis::div);
			y = term();
			// TODO: compute (DIV,x,y) and store in x
		}
		else
		{
			break;
		}
	}
	return x;
}

Result Parser::factor()
{
	Result x;
	if(scan.tk == LexAnalysis::open_paren)
	{
		mustParse(LexAnalysis::open_paren);
		x = expression();
		mustParse(LexAnalysis::close_paren);
	}
	else if (scan.tk == LexAnalysis::num_tk
			|| scan.tk == LexAnalysis::id_tk
			|| scan.tk == LexAnalysis::call)
	{
		x = value();
	}
	else{
		err();
	}
	return x;
}

Result Parser::value()
{
	if (scan.tk == LexAnalysis::num_tk)
	{
		mustParse(LexAnalysis::num_tk);
		return Result(Result::constKind, scan.num);
	}
	else if (scan.tk == LexAnalysis::call)
	{
		return callStatement();
	}
	return lvalue();
}

Result Parser::lvalue()
{
	mustParse(LexAnalysis::id_tk);
	while (scan.tk == LexAnalysis::open_bracket)
	{
		mustParse(LexAnalysis::open_bracket);
		expression();
		mustParse(LexAnalysis::close_bracket);
	}
	// TODO: addr of var
	return Result(Result::var, 0);
}

void Parser::compute(Op op, Result& x, Result& y)
{
	if(x.kind == Result::constKind && y.kind == Result::constKind)
	{
		switch(op)
		{
		case add:
			x.val += y.val;
			break;
		case sub:
			x.val -= y.val;
			break;
		case mul:
			x.val -= y.val;
			break;
		case div:
			x.val -= y.val;
			break;
		}
	}
}

void Parser::mustParse(LexAnalysis::Token tk) {
	if (scan.tk == tk)
	{
		scan.next();
	}
	else
	{
		std::cerr << "expected token '" << LexAnalysis::tkToStr(tk) << "' but found token '"
				<< LexAnalysis::tkToStr(scan.tk) << "' in " << scan.fname << ":" << scan.linenum << std::endl;
		exit(1);
	}
}

void Parser::err()
{
	std::cerr << "unexpected token '" << LexAnalysis::tkToStr(scan.tk)
			<< "' in " << scan.fname << ":" << scan.linenum << std::endl;
	exit(1);
}

void Parser::assignId(std::string s)
{
//	varMap[s] = valCount++;
}
