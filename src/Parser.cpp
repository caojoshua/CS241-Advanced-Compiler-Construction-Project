/*
 * Parser.cpp
 * Author: Joshua Cao
 */

#include "Parser.h"

Parser::Parser(char const* s) :
	scan(s), stackOffset(0), func(nullptr), currBB(nullptr), joinBB(nullptr)
{
	scan.next();
	pushMap();
}

SSA::Program& Parser::parse() {
	mustParse(LexAnalysis::main);
	declarationList();
	func = new SSA::Func("main");
	currBB = new SSA::BasicBlock();
	functionBody();
	mustParse(LexAnalysis::period);
	emitBB(currBB);
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
	func = new SSA::Func(scan.id);
	mustParse(LexAnalysis::id_tk);
	currBB = new SSA::BasicBlock();
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
	emitBB(currBB);
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
	assignVarValue(scan.id, new SSA::ConstOperand(0));
	mustParse(LexAnalysis::id_tk);
	while (scan.tk == LexAnalysis::comma)
	{
		mustParse(LexAnalysis::comma);
		assignVarValue(scan.id, new SSA::ConstOperand(0));
		mustParse(LexAnalysis::id_tk);
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
//	arrayMapStack[scan.id] = Array(len);
	mustParse(LexAnalysis::id_tk);
	while (scan.tk == LexAnalysis::comma)
	{
		mustParse(LexAnalysis::comma);
//		arrayMapStack[scan.id] = Array(len);
		mustParse(LexAnalysis::id_tk);
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
	SSA::BasicBlock* orig = currBB;
	emitBB(orig);
	currBB = new SSA::BasicBlock();
	orig->setLeft(currBB);
	joinBB = currBB;
	SSA::BasicBlock* oldJoin = joinBB;
	emitBB(joinBB);
	conditional();

	mustParse(LexAnalysis::do_tk);
	currBB = new SSA::BasicBlock();
	joinBB->setLeft(currBB);
	pushMap();
	statementList();
	insertPhisIntoPhiList();
	popMap();
	joinBB = oldJoin;
	currBB->setRight(joinBB);
	mustParse(LexAnalysis::od);

	emitBB(currBB);
	insertPhisIntoJoinBB();
	joinPhiList.clear();
	currBB = new SSA::BasicBlock();
	joinBB->setRight(currBB);
}

/*
 * The algorithm for generating if follows these steps (not exactly in order)
 * 1. emit code for if and else-if blocks
 * 2. insert phis after generating each block using block's
 * 	  varmap, which contains all assignments in the node, into the phi list
 * 3. insert phis into the join basic block using phi list
 */
void Parser::ifStatement()
{
	mustParse(LexAnalysis::if_tk);
	conditional();
	mustParse(LexAnalysis::then);

	emitBB(currBB);
	SSA::BasicBlock* origBB = currBB;
	currBB = new SSA::BasicBlock();
	joinBB = new SSA::BasicBlock();
	origBB->setLeft(currBB);
	currBB->setRight(joinBB);

	pushMap();
	joinPhiList.clear();
	statementList();
	insertPhisIntoPhiList();
	popMap();
	emitBB(currBB);
	if (scan.tk == LexAnalysis::else_tk)
	{
		mustParse(LexAnalysis::else_tk);
		currBB = new SSA::BasicBlock();
		origBB->setRight(currBB);
		pushMap();
		statementList();
		insertPhisIntoPhiList();
		popMap();
		emitBB(currBB);
		currBB->setLeft(joinBB);
	}
	else
	{
		origBB->setRight(joinBB);
	}
	currBB = joinBB;
	insertPhisIntoJoinBB();
	joinPhiList.clear();
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
	currBB->emit(new SSA::Instruction(SSA::cmp, x, y));
	currBB->emit(new SSA::Instruction(op));
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
	std::string funcName = scan.id;
	mustParse(LexAnalysis::id_tk);
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
	emit(currBB, ins);
	return new SSA::ValOperand(ins);
}

void Parser::assignment()
{
	mustParse(LexAnalysis::let);
	std::string varName = scan.id;
	mustParse(LexAnalysis::id_tk);
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
		assignVarValue(varName, op);
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
	SSA::Operand* op = getVarValue(scan.id);
	mustParse(LexAnalysis::id_tk);
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
	emit(currBB, instruction->getInstruction());
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

void Parser::pushMap()
{
	std::unordered_map<std::string, SSA::Operand*> varMap;
	std::unordered_map<std::string, Array> arrayMap;
	pushMap(varMap, arrayMap);
}

void Parser::pushMap(std::unordered_map<std::string, SSA::Operand*> varMap,
					std::unordered_map<std::string, Array> arrayMap)
{
	varMapStack.insert(varMapStack.cbegin(), varMap);
	arrayMapStack.insert(arrayMapStack.cbegin(), arrayMap);
}

void Parser::popMap()
{
	varMapStack.pop_front();
	arrayMapStack.pop_front();
}

void Parser::assignVarValue(std::string id, SSA::Operand *value)
{
	varMapStack.front()[id] = value;
}

void Parser::assignArrayValue(std::string id, SSA::Operand *value, int offset)
{
}

SSA::Operand* Parser::getVarValue(std::string id)
{
	for (std::unordered_map<std::string, SSA::Operand*> map : varMapStack)
	{
		if (map.find(id) != map.end())
		{
			if (map[id])
			{
				return map[id];
			}
			else
			{
				std::cerr << scan.fname << ":" << scan.linenum
						<< ": undefined variable " << id << std::endl;
				exit(1);
			}
		}
	}
	std::cerr << scan.fname << ":" << scan.linenum
						<< ": undeclared variable " << id << std::endl;
				exit(1);
}

SSA::Operand* Parser::getArrayValue(std::string id, int index)
{
//	if (arrayMapStack.find(id) == arrayMapStack.end())
//	{
//		std::cerr << scan.fname << ":" << scan.linenum
//				<< ": undeclared array variable " << id << std::endl;
//		exit(1);
//	}
//	return arrayMapStack[id].getOperand(index);
	return nullptr;
}

/*
 * - Store assignments of the top layer variable mapping into the phi list.
 * - This function should be called AFTER generating a control flow basic block
 *   and BEFORE popping the basic block's variable mapping.
 * - This avoids having to update the same phi instruction in the case of
 * 	 multiple assigns of the same variable.
 */
void Parser::insertPhisIntoPhiList()
{
	for (std::pair<std::string, SSA::Operand*> pair : varMapStack.front())
	{
		std::string varName = pair.first;
		SSA::Operand* operand = pair.second;
		bool foundPhi = false;
		for (SSA::PhiInstruction* phi : joinPhiList)
		{
			if (pair.first == phi->getVarName())
			{
				if (!phi->getOperand1())
				{
					phi->setOperand1(operand);
				}
				else if (!phi->getOperand2())
				{
					phi->setOperand2(operand);
				}
				foundPhi = true;
				break;
			}
		}
		if (!foundPhi)
		{
			joinPhiList.push_back(new SSA::PhiInstruction(operand, varName));
		}
	}
}


void Parser::updateReferencesToPhi()
{
//	for (std::pair<std::string, SSA::Instruction*> phi : joinPhiList)
//	{
//		SSA::Operand* old = getVarValue(phi.first);
//		SSA::Operand* copy = old;
//		*old = SSA::Operand(*old);
//		copy = phi.second;
//	}
}


/*
 * - Insert phis from the phi list into the join basic block.
 * - This function should be called after inserting phis into the phi list
 *   for each control flow block and popping their maps.
 * - In the case where a phi only has one argument, this function will insert
 *   the variable's value from the original value mapping.
 */
void Parser::insertPhisIntoJoinBB()
{
	for (SSA::PhiInstruction* phi : joinPhiList)
	{
		std::string varName = phi->getVarName();
		if (!phi->getOperand1())
		{
			phi->setOperand1(getVarValue(varName));
		}
		if (!phi->getOperand2())
		{
			phi->setOperand2(getVarValue(varName));
		}
		joinBB->emit(phi);
		assignVarValue(varName, new SSA::ValOperand(phi));
	}
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
