/*
 * Parser.cpp
 * Author: Joshua Cao
 */

#include "Parser.h"

Parser::Parser(char const* s) : scan(s), stackOffset(0), func(nullptr), bb(nullptr)
{
	scan.next();
}

SSA::Program& Parser::parse() {
	mustParse(LexAnalysis::main);
	declarationList();
	func = new SSA::Func("main");
	bb = new SSA::BasicBlock();
	functionBody();
	mustParse(LexAnalysis::period);
	emitBB(bb);
	IR.emitMain(func);
	return IR;
}

int Parser::Array::totalOffset = 0;

Parser::Array::Array(int length) : offset(totalOffset)
{
	vals.resize(length);
	for (int i = 0; i < vals.size(); ++i)
	{
		vals[i] = new SSA::ConstOperand(0);
	}
	totalOffset -= length * 4;
}

int Parser::Array::getOffset()
{
	return offset;
}

void Parser::Array::assign(int index, SSA::Operand* operand)
{
	vals[index] = operand;
}

SSA::Operand* Parser::Array::getOperand(int index)
{
	return vals[index];
}

// TODO: store function somewhere, still not sure how to handle functions
void Parser::function()
{
	mustParse(LexAnalysis::func);
	mustParse(LexAnalysis::id_tk);
	func = new SSA::Func(scan.id);
	bb = new SSA::BasicBlock();
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
	emitBB(bb);
	emitFunc();
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
	varMap[scan.id] = new SSA::ConstOperand(0);
	while (scan.tk == LexAnalysis::comma)
	{
		mustParse(LexAnalysis::comma);
		mustParse(LexAnalysis::id_tk);
		varMap[scan.id] = new SSA::ConstOperand(0);
	}
}

// TODO: store array in symbol table
void Parser::arrayDeclartion() {
	mustParse(LexAnalysis::array);
	int len = 1;
	do
	{
		mustParse(LexAnalysis::open_bracket);
		mustParse(LexAnalysis::num_tk);
		len *= scan.num;
		mustParse(LexAnalysis::close_bracket);
	} while (scan.tk == LexAnalysis::open_bracket);
	mustParse(LexAnalysis::id_tk);
	arrayMap[scan.id] = Array(len);
	while (scan.tk == LexAnalysis::comma)
	{
		mustParse(LexAnalysis::comma);
		mustParse(LexAnalysis::id_tk);
		arrayMap[scan.id] = Array(len);
	}
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
void Parser::conditional()
{
	SSA::Opcode op;
	SSA::Operand* x = expression();
	switch (scan.tk)
	{
	case LexAnalysis::e:
		mustParse(LexAnalysis::e);
		op = SSA::bne;
		break;
	case LexAnalysis::ne:
		mustParse(LexAnalysis::ne);
		op = SSA::beq;
		break;
	case LexAnalysis::lt:
		mustParse(LexAnalysis::lt);
		op = SSA::bge;
		break;
	case LexAnalysis::gt:
		mustParse(LexAnalysis::gt);
		op = SSA::ble;
		break;
	case LexAnalysis::lte:
		mustParse(LexAnalysis::lte);
		op = SSA::bgt;
		break;
	case LexAnalysis::gte:
		mustParse(LexAnalysis::gte);
		op = SSA::blt;
		break;
	default:
		err();
	}
	SSA::Operand* y = expression();
}

void Parser::returnStatement()
{
	mustParse(LexAnalysis::return_tk);
	if (scan.tk == LexAnalysis::id_tk
			|| scan.tk == LexAnalysis::num_tk
			|| scan.tk == LexAnalysis::call)
	{
		expression();
	}
}

SSA::Operand* Parser::callStatement()
{
	mustParse(LexAnalysis::call);
	mustParse(LexAnalysis::id_tk);
	std::string funcName = scan.id;
	std::list<SSA::Operand*> args;
	if (scan.tk == LexAnalysis::open_paren)
	{
		mustParse(LexAnalysis::open_paren);
		if (scan.tk != LexAnalysis::close_paren)
		{
			args.push_back(expression());
			while (scan.tk == LexAnalysis::comma)
			{
				mustParse(LexAnalysis::comma);
				expression();
			}
		}
		mustParse(LexAnalysis::close_paren);
	}
	SSA::CallOperand* callOp = new SSA::CallOperand(funcName, args);
	SSA::Instruction* ins = new SSA::Instruction(SSA::call, callOp);
	emit(bb, ins);
	return new SSA::ValOperand(ins);
}

void Parser::assignment()
{
	mustParse(LexAnalysis::let);
	mustParse(LexAnalysis::id_tk);
	std::string varName = scan.id;
	// array assignment
	if (scan.tk == LexAnalysis::open_bracket)
	{
		mustParse(LexAnalysis::open_bracket);
		SSA::Operand* index = expression();
		mustParse(LexAnalysis::close_bracket);
		while (scan.tk == LexAnalysis::open_bracket)
		{
			mustParse(LexAnalysis::open_bracket);
			index = compute(mul, index, expression());
			mustParse(LexAnalysis::close_bracket);
		}
		// TODO: add stack_pointer, scan.id_offset, index*4
		// emit add, adda and store operations
		mustParse(LexAnalysis::assign);
		expression();
	}
	// var assignment
	else
	{
		mustParse(LexAnalysis::assign);
		SSA::Operand* op = expression();
		varMap[varName] = op;
	}
}

SSA::Operand* Parser::expression()
{
	SSA::Operand* x = term();
	while (true)
	{
		if (scan.tk == LexAnalysis::add)
		{
			mustParse(LexAnalysis::add);
			x = compute(add, x, term());
		}
		else if (scan.tk == LexAnalysis::sub)
		{
			mustParse(LexAnalysis::sub);
			x = compute(sub, x, term());
		}
		else
		{
			break;
		}
	}
	return x;
}

SSA::Operand* Parser::term()
{
	SSA::Operand* x = factor();
	while (true)
	{
		if (scan.tk == LexAnalysis::mul)
		{
			mustParse(LexAnalysis::mul);
			x = compute(mul, x, factor());
		}
		else if (scan.tk == LexAnalysis::div)
		{
			mustParse(LexAnalysis::div);
			x = compute(div, x, factor());
		}
		else
		{
			break;
		}
	}
	return x;
}

SSA::Operand* Parser::factor()
{
	SSA::Operand* x;
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

SSA::Operand* Parser::value()
{
	if (scan.tk == LexAnalysis::num_tk)
	{
		mustParse(LexAnalysis::num_tk);
		return new SSA::ConstOperand(scan.num);
	}
	else if (scan.tk == LexAnalysis::call)
	{
		return callStatement();
	}
	else if (scan.tk == LexAnalysis::id_tk)
	{
		return lvalue();
	}
	err();
	return nullptr;
}

SSA::Operand* Parser::lvalue()
{
	mustParse(LexAnalysis::id_tk);
	SSA::Operand* op = getVarValue(scan.id);
	while (scan.tk == LexAnalysis::open_bracket)
	{
		mustParse(LexAnalysis::open_bracket);
		expression();
		mustParse(LexAnalysis::close_bracket);
	}
	return op;
}

SSA::Operand* Parser::compute(Opcode opcode, SSA::Operand* x, SSA::Operand* y)
{
	// constant folding
	if (x->getType() == SSA::Operand::constant && y->getType() == SSA::Operand::constant)
	{
		SSA::Operand* operand = nullptr;
		switch(opcode)
		{
		case add:
			operand = new SSA::ConstOperand(x->getConst() + y->getConst());
			break;
		case sub:
			operand = new SSA::ConstOperand(x->getConst() - y->getConst());
			break;
		case mul:
			operand = new SSA::ConstOperand(x->getConst() * y->getConst());
			break;
		case div:
			operand = new SSA::ConstOperand(x->getConst() / y->getConst());
			break;
		}
//		delete x;
//		delete y;
		return operand;
	}

	SSA::ValOperand* instruction = nullptr;
	switch(opcode)
	{
	case add:
		instruction = new SSA::ValOperand(new SSA::Instruction(SSA::add, x, y));
		break;
	case sub:
		instruction = new SSA::ValOperand(new SSA::Instruction(SSA::sub, x, y));
		break;
	case mul:
		instruction = new SSA::ValOperand(new SSA::Instruction(SSA::mul, x, y));
		break;
	case div:
		instruction = new SSA::ValOperand(new SSA::Instruction(SSA::div, x, y));
		break;
	}
	emit(bb, instruction->getInstruction());
	return instruction;
}

void Parser::mustParse(LexAnalysis::Token tk) {
	if (scan.tk == tk)
	{
		scan.next();
	}
	else
	{
		std::cerr << scan.fname << ":" << scan.linenum <<
				": expected token '" << LexAnalysis::tkToStr(tk)
				<< "' but found token '" << LexAnalysis::tkToStr(scan.tk) << std::endl;
		exit(1);
	}
}

void Parser::err()
{
	std::cerr << scan.fname << ":" << scan.linenum <<
			": unexpected token '" << LexAnalysis::tkToStr(scan.tk) << "' in " << std::endl;
	exit(1);
}

void Parser::emitFunc()
{
	if(func)
	{
		IR.emit(func);
	}
}

void Parser::emitBB(SSA::BasicBlock* bb)
{
	if (func && bb)
	{
		func->emit(bb);
	}
}
void Parser::emit(SSA::BasicBlock* bb, SSA::Instruction* ins)
{
	if (bb && ins)
	{
		bb->emit(ins);
	}
}

SSA::Operand* Parser::getVarValue(std::string id)
{
	if (varMap.find(id) == varMap.end())
	{
		std::cerr << scan.fname << ":" << scan.linenum
				<< ": undeclared variable " << id << std::endl;
		exit(1);
	}
	else if (!varMap[id])
	{
		std::cerr << scan.fname << ":" << scan.linenum
				<< ": undefined variable " << id << std::endl;
		exit(1);
	}
	return varMap[id];
}

SSA::Operand* Parser::getArrayValue(std::string id, int index)
{
	if (arrayMap.find(id) == arrayMap.end())
	{
		std::cerr << scan.fname << ":" << scan.linenum
				<< ": undeclared array variable " << id << std::endl;
		exit(1);
	}
	return arrayMap[id].getOperand(index);
}
