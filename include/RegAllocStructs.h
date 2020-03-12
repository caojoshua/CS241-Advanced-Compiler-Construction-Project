/*
 * RegAllocStructs.h
 * Author: Joshua Cao
 */

#ifndef INCLUDE_REGALLOCSTRUCTS_H_
#define INCLUDE_REGALLOCSTRUCTS_H_

#include <algorithm>
#include "SSA.h"

class IntervalList
{
public:
	class Interval
	{
	private:
		std::list<std::pair<int, int>> ranges;
	public:
		Interval();
		Interval(int from, int to);
		std::list<std::pair<int, int>> getRanges() const;
		void addRange(int from, int to);
		void setFrom(int from);
		bool intersects(Interval other) const;
	};
private:
	std::map<SSA::Instruction*, Interval> intervals;
public:
	std::list<std::pair<int, int>> getRanges(SSA::Instruction* i) const;
	void setFrom(SSA::Instruction* i, int from);
	void addRange(SSA::Instruction* i, int from, int to);
	void addRange(SSA::Operand* o, int from, int to);
	// void buildInterferenceGraph();
};

#endif /* INCLUDE_REGALLOCSTRUCTS_H_ */
