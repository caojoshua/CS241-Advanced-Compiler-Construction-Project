/*
 * Parser.cpp
 * Author: Joshua Cao
 */

#include "Parser.h"

Parser::Parser(char const *s) :
		scan(s), module(new SSA::Module()), func(nullptr), currBB(nullptr), joinBB(
				nullptr)
{
	scan.next();
	pushVarMap();
	pushCSEmap();
}

SSA::Module* Parser::parse()
{
	SSA::Instruction::resetId();
	Array::resetTotalOffset();
	func = new SSA::Function(module, "main");
	mustParse(LexAnalysis::main);
	currBB = new SSA::BasicBlock();
	emitBB(currBB);
	declarationList();
	functionBody();
	mustParse(LexAnalysis::period);
	module->emit(func);
	return module;
}

int Parser::Array::totalOffset = 0;

Parser::Array::Array(Parser *parser, std::vector<int> dims) :
		parser(parser), dims(dims)
{
	int length = 1;
	for (int dim : dims)
	{
		length *= dim;
	}
	totalOffset -= length * INT_SIZE;
	offset = totalOffset;
	parser->func->setLocalVariableOffset(totalOffset);
}

Parser::Array& Parser::Array::operator=(const Array other)
{
	offset = other.offset;
	dims = other.dims;
	return *this;
}

void Parser::Array::resetTotalOffset()
{
	totalOffset = 0;
}

int Parser::Array::getOffset()
{
	return offset;
}

std::vector<int> Parser::Array::getDims() const
{
	return dims;
}

// TODO: store function somewhere, still not sure how to handle functions
void Parser::function()
{
	mustParse(LexAnalysis::func);
	SSA::Function* oldFunc = func;
	SSA::BasicBlock* oldCurrBB = currBB;
	func = new SSA::Function(module, scan.id);
	emitFunc();
	mustParse(LexAnalysis::id_tk);
	currBB = new SSA::BasicBlock();
	emitBB(currBB);
	if (scan.tk == LexAnalysis::open_paren)
	{
		mustParse(LexAnalysis::open_paren);
		if (scan.tk == LexAnalysis::id_tk)
		{
			mustParse(LexAnalysis::id_tk);
			SSA::Instruction* pop = new SSA::Instruction(SSA::pop);
			assignVarValue(scan.id, new SSA::ValOperand(pop));
			emit(currBB, pop);
			while (scan.tk == LexAnalysis::comma)
			{
				mustParse(LexAnalysis::comma);
				SSA::Instruction* pop = new SSA::Instruction(SSA::pop);
				assignVarValue(scan.id, new SSA::ValOperand(pop));
				emit(currBB, pop);
				mustParse(LexAnalysis::id_tk);
			}
		}
		mustParse(LexAnalysis::close_paren);
	}
	mustParse(LexAnalysis::semicolon);
	declarationList();
	functionBody();
	currBB = oldCurrBB;
	func = oldFunc;
	mustParse(LexAnalysis::semicolon);
}

void Parser::declarationList()
{
	while (true)
	{
		switch (scan.tk)
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

void Parser::varDeclareList()
{
	SSA::Instruction* i = new SSA::Instruction(SSA::constant, new SSA::ConstOperand(0));
	SSA::Instruction* cse = cseCheck(i);
	if (cse == i)
	{
		emit(currBB, i);
	}
	i = cse;
	SSA::Operand* val = new SSA::ValOperand(i);
	assignVarValue(scan.id, val);
	mustParse(LexAnalysis::id_tk);
	while (scan.tk == LexAnalysis::comma)
	{
		mustParse(LexAnalysis::comma);
		SSA::Instruction* i = new SSA::Instruction(SSA::constant, new SSA::ConstOperand(0));
		if (cse == i)
		{
			emit(currBB, i);
		}
		i = cse;
		assignVarValue(scan.id, val);
		mustParse(LexAnalysis::id_tk);
	}
	delete val;
}

void Parser::arrayDeclartion()
{
	mustParse(LexAnalysis::array);
	std::vector<int> dims;
	do
	{
		mustParse(LexAnalysis::open_bracket);
		mustParse(LexAnalysis::num_tk);
		dims.push_back(scan.num);
		mustParse(LexAnalysis::close_bracket);
	} while (scan.tk == LexAnalysis::open_bracket);

	arrayMap[scan.id] = Array(this, dims);
	mustParse(LexAnalysis::id_tk);
	while (scan.tk == LexAnalysis::comma)
	{
		mustParse(LexAnalysis::comma);
		arrayMap[scan.id] = Array(this, dims);
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
	if (scan.tk != LexAnalysis::let && scan.tk != LexAnalysis::return_tk
			&& scan.tk != LexAnalysis::call && scan.tk != LexAnalysis::while_tk
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
	switch (scan.tk)
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
	SSA::BasicBlock *orig = currBB;
	currBB = new SSA::BasicBlock(true);
	emitBB(currBB);
	linkBB(orig, currBB);
	joinBB = currBB;
	SSA::BasicBlock *oldJoin = joinBB;
	pushUseChain();
	conditional();

	mustParse(LexAnalysis::do_tk);
	currBB = new SSA::BasicBlock();
	emitBB(currBB);
	linkBB(joinBB, currBB);
	pushVarMap();
	pushCSEmap();
	statementList();
	insertPhis(currBB, oldJoin);
	popVarMap();
	popCSEmap();
	joinBB = oldJoin;
	linkBB(currBB, joinBB);
	mustParse(LexAnalysis::od);

	commitPhis(joinBB, true);
	popUseChain();
	currBB = new SSA::BasicBlock();
	emitBB(currBB);
	linkBB(joinBB, currBB);
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

	SSA::BasicBlock *origBB = currBB;
	currBB = new SSA::BasicBlock();
	emitBB(currBB);
	joinBB = new SSA::BasicBlock();
	joinBB->setParent(func);
	SSA::BasicBlock *oldJoin = joinBB;
	linkBB(origBB, currBB);

	pushVarMap();
	pushCSEmap();
	statementList();
	insertPhis(currBB, oldJoin);
	popVarMap();
	popCSEmap();
	linkBB(currBB, oldJoin);

	if (scan.tk == LexAnalysis::else_tk)
	{
		mustParse(LexAnalysis::else_tk);
		currBB = new SSA::BasicBlock();
		emitBB(currBB);
		linkBB(origBB, currBB);
		pushVarMap();
		pushCSEmap();
		statementList();
		insertPhis(currBB, oldJoin);
		popVarMap();
		popCSEmap();
		joinBB = oldJoin;
		linkBB(currBB, joinBB);
	} else
	{
		joinBB = oldJoin;
		linkBB(origBB, joinBB);
	}
	currBB = joinBB;
	commitPhis(joinBB);
	emitBB(currBB);
	mustParse(LexAnalysis::fi);
}

// TODO: compute results of conditional and store in x
void Parser::conditional()
{
	SSA::Opcode op;
	SSA::Operand *x = expression();
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

	SSA::Operand *y = expression();
	SSA::Instruction *ins = new SSA::Instruction(SSA::cmp, x, y);
	SSA::Instruction *cse = cseCheck(ins);
	if (cse != ins)
	{
		return;
	}
	ins = cse;

	currBB->emit(ins);
	currBB->emit(new SSA::Instruction(op, new SSA::ValOperand(ins)));

	if (!useChain.empty())
	{
		insertIntoUseChain(x, ins);
		insertIntoUseChain(y, ins);
	}
}

void Parser::returnStatement()
{
	mustParse(LexAnalysis::return_tk);
	if (scan.tk == LexAnalysis::id_tk || scan.tk == LexAnalysis::num_tk
			|| scan.tk == LexAnalysis::call)
	{
		SSA::Operand* returnVal = expression();
		emit(currBB, new SSA::Instruction(SSA::ret, returnVal));
		func->setIsVoid(false);
	} else
	{
		emit(currBB, new SSA::Instruction(SSA::ret));
		func->setIsVoid(true);
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
				args.push_back(expression());
			}
		}
		mustParse(LexAnalysis::close_paren);
	}

	SSA::Function* f = module->getFunction(funcName);
	if (!f)
	{
		std::cerr << scan.fname << ":" << scan.linenum << ": undeclared function '"
					<< funcName << "'" << std::endl;
		exit(1);
	}

	SSA::CallOperand *callOp = new SSA::CallOperand(f, args);
	SSA::Instruction *ins = new SSA::Instruction(SSA::call, callOp);

	if (!useChain.empty())
	{
		for (SSA::Operand *arg : args)
		{
			insertIntoUseChain(arg, ins);
		}
	}

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
		SSA::Operand *memLoc = arrayIndexReference();
		mustParse(LexAnalysis::assign);
		SSA::Operand* exp = expression();
		SSA::Instruction* adda = new SSA::Instruction(SSA::adda, new SSA::GlobalRegOperand, memLoc);
		SSA::Instruction* cse = cseCheck(adda);
		if (adda == cse)
		{
			emit(currBB, adda);
		}
		adda = cse;
		SSA::Instruction* store = new SSA::Instruction(SSA::store, new SSA::ValOperand(adda), exp);
		memoryKill(store);
		emit(currBB, store);
	}
	// var assignment
	else
	{
		mustParse(LexAnalysis::assign);
		SSA::Operand *op = expression();
		if (op->getType() == SSA::Operand::constant)
		{
			SSA::Instruction* i = new SSA::Instruction(SSA::constant, op);
			SSA::Instruction* cse = cseCheck(i);
			if (cse == i)
			{
				emit(currBB, i);
			}
			i = cse;
			op = new SSA::ValOperand(i);
		}
		assignVarValue(varName, op);
	}
}

SSA::Operand* Parser::expression()
{
	SSA::Operand *x = term();
	while (true)
	{
		if (scan.tk == LexAnalysis::add)
		{
			mustParse(LexAnalysis::add);
			x = compute(add, x, term());
		} else if (scan.tk == LexAnalysis::sub)
		{
			mustParse(LexAnalysis::sub);
			x = compute(sub, x, term());
		} else
		{
			break;
		}
	}
	return x;
}

SSA::Operand* Parser::term()
{
	SSA::Operand *x = factor();
	while (true)
	{
		if (scan.tk == LexAnalysis::mul)
		{
			mustParse(LexAnalysis::mul);
			x = compute(mul, x, factor());
		} else if (scan.tk == LexAnalysis::div)
		{
			mustParse(LexAnalysis::div);
			x = compute(div, x, factor());
		} else
		{
			break;
		}
	}
	return x;
}

SSA::Operand* Parser::factor()
{
	SSA::Operand *x;
	if (scan.tk == LexAnalysis::open_paren)
	{
		mustParse(LexAnalysis::open_paren);
		x = expression();
		mustParse(LexAnalysis::close_paren);
	} else if (scan.tk == LexAnalysis::num_tk || scan.tk == LexAnalysis::id_tk
			|| scan.tk == LexAnalysis::call)
	{
		x = value();
	} else
	{
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
	} else if (scan.tk == LexAnalysis::call)
	{
		return callStatement();
	} else if (scan.tk == LexAnalysis::id_tk)
	{
		return lvalue();
	}
	err();
	return nullptr;
}

SSA::Operand* Parser::lvalue()
{
	std::string name = scan.id;
	mustParse(LexAnalysis::id_tk);
	if (scan.tk == LexAnalysis::open_bracket)
	{
		SSA::Operand *memLoc = arrayIndexReference();
		SSA::Instruction* adda = new SSA::Instruction(SSA::adda, new SSA::GlobalRegOperand(), memLoc);
		SSA::Instruction* addaCse = cseCheck(adda);
		if (adda == addaCse)
		{
			emit(currBB, adda);
		}
		adda = addaCse;

		SSA::Instruction *ins = new SSA::Instruction(SSA::load, new SSA::ValOperand(adda));
		SSA::Instruction* cse = cseCheck(ins);
		if (cse == ins)
		{
			emit(currBB, ins);
		}
		return new SSA::ValOperand(cse);
	} else
	{
		return getVarValue(name);
	}
}

SSA::Operand* Parser::arrayIndexReference()
{
	Array arr = arrayMap[scan.id];
	std::vector<SSA::Operand*> accessDims;

	// parse the access dimensions
	do
	{
		mustParse(LexAnalysis::open_bracket);
		accessDims.push_back(expression());
		mustParse(LexAnalysis::close_bracket);
	} while (scan.tk == LexAnalysis::open_bracket);

	int numDims = accessDims.size();
	std::vector<int> arrDims = arr.getDims();
	int expectedNumDims = arrDims.size();

	if (numDims != expectedNumDims)
	{
		std::cerr << scan.fname << ":" << scan.linenum << " Array has "
				<< expectedNumDims << " but tried to access with " << numDims
				<< " dimensions" << std::endl;
		exit(1);
	}

	// map multidimensional indices to flat row-order index
	int lastDim = numDims - 1;
	int prod = arrDims[lastDim];
	SSA::Operand *index = accessDims[lastDim];
	for (int i = lastDim - 1; i >= 0; --i)
	{
		SSA::Operand *o = compute(mul, accessDims[i],
				new SSA::ConstOperand(prod));
		index = compute(add, index, o);
		prod *= arrDims[i];
	}

	index = compute(mul, index, new SSA::ConstOperand(INT_SIZE));
	index = compute(add, new SSA::ConstOperand(arr.getOffset()), index);
	return index;
}

SSA::Operand* Parser::compute(Opcode opcode, SSA::Operand *x, SSA::Operand *y)
{
	// constant folding
	if (x->getType() == SSA::Operand::constant
			&& y->getType() == SSA::Operand::constant)
	{
		SSA::Operand *operand = nullptr;
		switch (opcode)
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

	SSA::Opcode op;
	switch (opcode)
	{
	case add:
		op = SSA::add;
		break;
	case sub:
		op = SSA::sub;
		break;
	case mul:
		op = SSA::mul;
		break;
	case div:
		op = SSA::div;
		break;
	}
	SSA::Instruction *ins = new SSA::Instruction(op, x, y);

	SSA::Instruction *cse = cseCheck(ins);
	if (cse != ins)
	{
		return new SSA::ValOperand(cse);
	}
	ins = cse;

	if (!useChain.empty())
	{
		insertIntoUseChain(x, ins);
		insertIntoUseChain(y, ins);
	}

	emit(currBB, ins);
	return new SSA::ValOperand(ins);
}

void Parser::mustParse(LexAnalysis::Token tk)
{
	if (scan.tk == tk)
	{
		scan.next();
	} else
	{
		std::cerr << scan.fname << ":" << scan.linenum << ": expected token '"
				<< LexAnalysis::tkToStr(tk) << "' but found token '"
				<< LexAnalysis::tkToStr(scan.tk) << std::endl;
		exit(1);
	}
}

void Parser::err()
{
	std::cerr << scan.fname << ":" << scan.linenum << ": unexpected token '"
			<< LexAnalysis::tkToStr(scan.tk) << "'" << std::endl;
	exit(1);
}

void Parser::linkBB(SSA::BasicBlock *pred, SSA::BasicBlock *succ)
{
	pred->addSuccessor(succ);
	succ->addPredecessor(pred);
}

void Parser::pushVarMap()
{
	pushVarMap(std::unordered_map<std::string, SSA::Operand*>());
}

void Parser::pushVarMap(std::unordered_map<std::string, SSA::Operand*> varMap)
{
	varMapStack.insert(varMapStack.cbegin(), varMap);
}

void Parser::popVarMap()
{
	varMapStack.pop_front();
}

void Parser::assignVarValue(std::string id, SSA::Operand *value)
{
	varMapStack.front()[id] = value->clone();
}

SSA::Operand* Parser::getVarValue(std::string id, bool fromExpression)
{
	for (std::unordered_map<std::string, SSA::Operand*> map : varMapStack)
	{
		if (map.find(id) != map.end())
		{
			SSA::Operand *varValue = map[id];
			if (varValue)
			{
				return varValue;
			} else
			{
				std::cerr << scan.fname << ":" << scan.linenum
						<< ": undefined variable " << id << std::endl;
				exit(1);
			}
		}
	}
	std::cerr << scan.fname << ":" << scan.linenum << ": undeclared variable "
			<< id << std::endl;
	exit(1);
}

void Parser::pushUseChain()
{
	useChain.push_front(
			std::unordered_map<SSA::Operand*, std::list<SSA::Instruction*>>());
}

void Parser::popUseChain()
{
	if (!useChain.empty())
	{
		useChain.pop_front();
	}
}

/*
 * assumes use chain stack is not empty
 * TODO: I think we should only be updating the front of the use chain, create a testcase for this
 */
void Parser::insertIntoUseChain(SSA::Operand *operand, SSA::Instruction *ins)
{
	for (std::unordered_map<SSA::Operand*, std::list<SSA::Instruction*>> &useChainLevel : useChain)
	{
		if (operand)
		{
			if (useChainLevel.find(operand) == useChainLevel.cend())
			{
				useChainLevel[operand] = std::list<SSA::Instruction*>();
			}
			useChainLevel[operand].push_back(ins);
		}
	}
}

void Parser::replaceOldOperandWithPhi(SSA::Operand *oldOperand,
		SSA::Operand *newOperand, SSA::Instruction *ins, bool left)
{
	SSA::Operand *currOperand = nullptr;
	if (left)
	{
		currOperand = ins->getOperand1();
	} else
	{
		currOperand = ins->getOperand2();
	}
	if (currOperand)
	{
		switch (currOperand->getType())
		{
		case SSA::Operand::val:
			if (currOperand == oldOperand)
			{
				if (left)
				{
					ins->setOperand1(newOperand);
				} else
				{
					ins->setOperand2(newOperand);
				}
			}
			break;
		case SSA::Operand::call:
		case SSA::Operand::phi:
			std::list<SSA::Operand*> args = currOperand->getArgs();
			for (SSA::Operand *&arg : args)
			{
				if (arg == oldOperand)
				{
					currOperand->replaceArg(arg, newOperand);
				}
			}
			break;
		}
	}
}

void Parser::insertPhis(SSA::BasicBlock *from, SSA::BasicBlock *to)
{
	for (std::pair<std::string, SSA::Operand*> pair : varMapStack.front())
	{
		std::string varName = pair.first;
		SSA::Operand *operand = pair.second;
		bool foundPhi = false;
		for (SSA::Instruction *ins : to->getInstructions())
		{
			SSA::Operand *phi = ins->getOperand1();
			if (ins->getOpcode() == SSA::phi && pair.first == phi->getVarName())
			{
				phi->addPhiArg(from, operand);
				foundPhi = true;
				break;
			}
		}
		if (!foundPhi)
		{
			SSA::PhiOperand *phi = new SSA::PhiOperand(varName, from, operand);
			to->emitFront(new SSA::Instruction(SSA::phi, phi));
		}
	}
}

void Parser::commitPhis(SSA::BasicBlock *b, bool loop)
{
	for (SSA::Instruction *ins : b->getInstructions())
	{
		if (ins->getOpcode() == SSA::phi)
		{
			SSA::Operand *phiOp = ins->getOperand1();
			std::map<SSA::BasicBlock*, SSA::Operand*> phiArgs =
					phiOp->getPhiArgs();
			std::string varName = phiOp->getVarName();
			SSA::Operand *prevValue = getVarValue(varName, false);

			// set phi operands to the previous value if not set
			for (SSA::BasicBlock *pred : b->getPredecessors())
			{
				if (phiArgs.find(pred) == phiArgs.cend())
				{
					phiOp->addPhiArg(pred, prevValue);
				}
			}

			SSA::ValOperand *newOperand = new SSA::ValOperand(ins);

			// propagate phi values
			if (loop && !useChain.empty())
			{
				for (std::unordered_map<SSA::Operand*,
						std::list<SSA::Instruction*>> useChainLevel : useChain)
				{
					for (std::pair<SSA::Operand*, std::list<SSA::Instruction*>> usePair : useChainLevel)
					{
						if (prevValue == usePair.first)
						{
							for (SSA::Instruction *i : usePair.second)
							{
								replaceOldOperandWithPhi(prevValue, newOperand,
										i, true);
								replaceOldOperandWithPhi(prevValue, newOperand,
										i, false);
							}
						}
					}
				}
			}

			// add args to use chain AFTER propagting so that the phi instruction
			// does not propagate itself into its own operands
			for (std::pair<SSA::BasicBlock*, SSA::Operand*> arg : phiOp->getPhiArgs())
			{
				insertIntoUseChain(arg.second, ins);
			}

			assignVarValue(varName, newOperand);
		}
	}
}

void Parser::pushCSEmap()
{
	cseStack.push_front(std::map<SSA::Opcode, std::list<SSA::Instruction*>>());
}

void Parser::popCSEmap()
{
	cseStack.pop_front();
}

SSA::Operand* Parser::getMemoryAccessOffset(SSA::Instruction* store) const
{
	if (store && (store->getOpcode() == SSA::store
			|| store->getOpcode() == SSA::load))
	{
		SSA::Operand* memOp = store->getOperand1();
		if (memOp->getType() == SSA::Operand::globalReg)
		{
			memOp = store->getOperand2();
		}
		if (memOp && memOp->getType() == SSA::Operand::val)
		{
			SSA::Instruction* adda = memOp->getInstruction();
			if (adda && adda->getOpcode() == SSA::adda)
			{
				SSA::Operand* op1 = adda->getOperand1();
				SSA::Operand* op2 = adda->getOperand2();
				SSA::Operand* nonGlobal = nullptr;
				if (op1->getType() == SSA::Operand::globalReg)
				{
					nonGlobal = op2;
				}
				else if (op2->getType() == SSA::Operand::globalReg)
				{
					nonGlobal = op1;
				}
				return nonGlobal;
			}
		}
	}
	return nullptr;
}

SSA::Instruction* Parser::cseCheck(SSA::Instruction *ins)
{
	SSA::Opcode op = ins->getOpcode();

	// check if same Instruction already exists
	for (std::map<SSA::Opcode, std::list<SSA::Instruction*>> map : cseStack)
	{
		if (map.find(op) != map.cend())
		{
			for (SSA::Instruction *cseIns : map[op])
			{
				if (ins->equals(cseIns))
				{
//					delete ins;
					return cseIns;
				}
			}
		}
	}

	// insert the new instruction into the CSE stack and return it
	std::map<SSA::Opcode, std::list<SSA::Instruction*>> &front =
			cseStack.front();
	if (front.find(op) == front.cend())
	{
		front[op] = std::list<SSA::Instruction*>();
	}
	front[op].push_back(ins);
	return ins;
}

/*
 * kill loads in the CSE stack
 * call when emitting a store instructions
 * if store offset is a constant, kill all unknown loads and load w same offset constant
 * if store offset is unknown, kill all loads
 *    - if we did array bound checking, we can just kill loads for that array
 */
void Parser::memoryKill(SSA::Instruction* i)
{
	if (i && i->getOpcode() == SSA::store)
	{
		SSA::Operand* offset = getMemoryAccessOffset(i);
		if (offset)
		{
			for (auto& cseLevel : cseStack)
			{
				for (auto& csePair : cseLevel)
				{
					if (csePair.first == SSA::load)
					{
						auto& insList = csePair.second;
						auto iter = insList.begin();
						while (iter != insList.end())
						{
							if (offset->getType() == SSA::Operand::constant)
							{
								SSA::Instruction* cseIns = *iter;
								SSA::Operand* cseOffset = getMemoryAccessOffset(cseIns);
								if (cseOffset && (cseOffset->getType() == SSA::Operand::val ||
										offset->equals(cseOffset)))
								{
									iter = insList.erase(iter);
									continue;
								}
							}
							else if (offset->getType() == SSA::Operand::val)
							{
								iter = insList.erase(iter);
								continue;
							}
							++iter;
						}
					}
				}
			}
		}
	}
}

void Parser::emitFunc()
{
	if (func)
	{
		module->emit(func);
	}
}

void Parser::emitBB(SSA::BasicBlock *bb)
{
	if (func && bb)
	{
		func->emit(bb);
	}
}

void Parser::emit(SSA::BasicBlock *bb, SSA::Instruction *ins)
{
	if (bb && ins)
	{
		bb->emit(ins);
	}

}
