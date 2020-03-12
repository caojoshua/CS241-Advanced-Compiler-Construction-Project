/*
 * RegAlloc.cpp
 * Author: Joshua Cao
 */

#include <RegAlloc.h>

void addOperandToLive(std::list<SSA::Instruction*>& live, SSA::Operand* o)
{
	if (o)
	{
		switch(o->getType())
		{
		case SSA::Operand::val:
			live.push_back(o->getInstruction());
			break;
		case SSA::Operand::phi:
		case SSA::Operand::call:
			for (SSA::Operand* arg : o->getArgs())
			{
				live.push_back(o->getInstruction());
			}
			break;
		}
	}
}

/*
 * WIMMER, C.,ANDFRANZ, M.
 * Linear scan register allocation on ssa form
 * Figure 4. BuildIntervals
 *
 * Question: don't we need to include phis in liveset in loop headers
 * to propage its liveness throughout loop?
 */
IntervalList buildIntervals(SSA::Func* f)
{
	IntervalList intervals;
	std::map<SSA::BasicBlock*, std::list<SSA::Instruction*>> liveIn;
	std::list<SSA::BasicBlock*> BBs = f->getBBs();

	// keep track of last lineIds to keep track of end of loop bodies
	std::map<SSA::BasicBlock*, int> endLineIds;

	// compute lineId, which is the number of instructions in the function
	uint lineId = 0;
	for (SSA::BasicBlock* b : BBs)
	{
		lineId += b->getInstructions().size();
	}
	--lineId;

	// iterate through basic blocks and instructions in reverse order
	for (std::list<SSA::BasicBlock*>::reverse_iterator it = BBs.rbegin(); it != BBs.rend(); ++it)
	{
		std::list<SSA::Instruction*> live;
		SSA::BasicBlock* b = *it;

		// compute live from successors
		for (SSA::BasicBlock* succ : b->getSuccessors())
		{
			// add visited successors' liveIns to live
			// and phi args associated with b
			if (liveIn.find(succ) != liveIn.cend())
			{
				for (SSA::Instruction* i : liveIn[succ])
				{
					live.push_back(i);
				}
				for (SSA::Instruction* i : succ->getInstructions())
				{
					if (i->getOpcode() == SSA::phi)
					{
						SSA::Operand* phiArg = i->getOperand1()->getPhiArg(b);
						if (phiArg && phiArg->getType() == SSA::Operand::val)
						{
							live.push_back(phiArg->getInstruction());
						}
					}
				}
			}
		}

		std::list<SSA::Instruction*> instructions = b->getInstructions();
		uint bFrom = lineId - instructions.size() + 1;
		uint bTo = lineId;
		endLineIds[b] = lineId;

		// add live range over basic block for each value in live
		for (SSA::Instruction* i : live)
		{
			intervals.addRange(i, bFrom, bTo);
		}

		// compute live ranges of operands
		for (std::list<SSA::Instruction*>::reverse_iterator ins_it = instructions.rbegin();
				ins_it != instructions.rend(); ++ins_it)
		{
			SSA::Instruction* ins = *ins_it;
			if (ins->getOpcode() != SSA::phi)
			{
				// output operand
				intervals.setFrom(ins, lineId);

				// input operand
				SSA::Operand* op1 = ins->getOperand1();
				SSA::Operand* op2 = ins->getOperand2();
				intervals.addRange(op1, bFrom, lineId);
				intervals.addRange(op2, bFrom, lineId);
				addOperandToLive(live, op1);
				addOperandToLive(live, op2);
			}
			live.remove(ins);
			ins->setId(lineId);
			--lineId;
		}

		// extend range of loop header live set to entire loop body
		if (b->isLoopHeader())
		{
			// predecessor that has already been visited is last node of loop body
			int loopBodyEndId = -1;
			for (SSA::BasicBlock* pred : b->getPredecessors())
			{
				if (endLineIds.find(pred) != endLineIds.cend())
				{
					if (endLineIds[pred] > loopBodyEndId)
					{
						loopBodyEndId = endLineIds[pred];
					}
				}
			}
			for (SSA::Instruction* liveIns : live)
			{
				intervals.addRange(liveIns, bFrom, loopBodyEndId);
			}
		}
		liveIn[b] = live;
	}

	// output debug
	for (SSA::BasicBlock* b : BBs)
	{
		for (SSA::Instruction* i : b->getInstructions())
		{
			std::cout << i->toStr() << ": ";
			for (std::pair<int, int> pair : intervals.getRanges(i))
			{
				std::cout << " (" << pair.first << ", " << pair.second << ")";
			}
			std::cout << std::endl;
		}
	}
	return intervals;
}

void allocateRegisters(SSA::Func* f)
{
	IntervalList intervals = buildIntervals(f);
	// build interference graph
	// graph coloring
}

void allocateRegisters(SSA::IntermediateRepresentation ir)
{
	allocateRegisters(ir.getMain());
	for (SSA::Func* f : ir.getFuncs())
	{
		allocateRegisters(f);
	}
}

