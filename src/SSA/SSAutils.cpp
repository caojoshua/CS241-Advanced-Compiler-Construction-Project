/*
 * SSAutils.cpp
 * Author: Joshua Cao
 */

#include "SSAutils.h"

#include "Instruction.h"

std::string SSA::opToStr(Opcode op)
{
	switch(op)
	{
	case add: 		return "add";
	case sub: 		return "sub";
	case mul: 		return "mul";
	case div: 		return "div";
	case cmp: 		return "cmp";
	case adda: 		return "adda";
	case load:		return "load";
	case store:		return "store";
	case move:		return "move";
	case phi:		return "phi";
	case end:		return "end";
	case bra:		return "bra";
	case bne:		return "bne";
	case beq:		return "beq";
	case ble:		return "ble";
	case blt:		return "blt";
	case bge:		return "bge";
	case bgt:		return "bgt";
	case read:		return "read";
	case write:		return "write";
	case writeNL:	return "writeNL";
	case call:		return "call";
	case ret:		return "ret";
	case pop:		return "pop";
	case constant:	return "constant";
	}
	return "";
}
