/*
 * RegAllocStrucs.c
 * Author: Joshua Cao
 */

# include "RegAllocStructs.h"

IntervalList::Interval::Interval()
{
}

IntervalList::Interval::Interval(int from, int to)
{
	addRange(from, to);
}

std::list<std::pair<int, int>> IntervalList::Interval::getRanges() const
{
	return ranges;
}

void IntervalList::Interval::addRange(int from, int to)
{
	if (to < from)
	{
		return;
	}

	// coalesce ranges that are adjacent or overlap
	for (std::pair<int, int>& range : ranges)
	{
		if (from - 1 <= range.second && to + 1 >= range.first)
		{
			range.first = std::min(range.first, from);
			range.second = std::max(range.second, to);
			return;
		}
	}

	ranges.push_back(std::pair<int, int>(from, to));
}

void IntervalList::Interval::setFrom(int from)
{
	for (std::pair<int, int>& range : ranges)
	{
		if (from > range.first)
		{
			range.first = from;
		}
	}
}

bool IntervalList::Interval::intersects(Interval other) const
{
	for (std::pair<int, int> thisRange : ranges)
	{
		for (std::pair<int, int> otherRange: other.ranges)
		{
			if (thisRange.first <= otherRange.second
					&& thisRange.second >= otherRange.first)
			{
				return true;
			}
		}
	}
	return false;
}

void IntervalList::addRange(SSA::Instruction *i, int from, int to)
{
	if (i)
	{
		intervals[i].addRange(from, to);
	}
}

void IntervalList::addRange(SSA::Operand *o, int from, int to)
{
	if (o)
	{
		switch(o->getType())
		{
		case SSA::Operand::val:
			addRange(o->getInstruction(), from, to);
			break;
		case SSA::Operand::phi:
		case SSA::Operand::call:
			for (SSA::Operand* arg : o->getArgs())
			{
				addRange(arg, from, to);
			}
			break;
		}
	}
}

std::list<std::pair<int, int>> IntervalList::getRanges(SSA::Instruction* i) const
{
	if (intervals.find(i) != intervals.cend())
	{
		return intervals.at(i).getRanges();
	}
	return std::list<std::pair<int, int>>();
}

void IntervalList::setFrom(SSA::Instruction *i, int from)
{
	if (intervals.find(i) != intervals.cend())
	{
		intervals[i].setFrom(from);
	}
}
