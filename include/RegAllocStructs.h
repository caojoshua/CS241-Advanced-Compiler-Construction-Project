/*
 * RegAllocStructs.h
 * Author: Joshua Cao
 */

#ifndef INCLUDE_REGALLOCSTRUCTS_H_
#define INCLUDE_REGALLOCSTRUCTS_H_

#include "SSA.h"
#include <algorithm>
#include <vector>
#include <unordered_map>

class InterferenceGraph
{
public:
	struct Node
	{
	public:
		Node();
		Node(int id);
		int id;
		std::list<SSA::Instruction*> edges;
	};
private:
	// edges stored in adjacency matrix and within nodes
	// G. J. Chaitin
	// Register Allocation & Spilling via Graph Coloring
	std::vector<std::vector<bool>> adjacencyMatrix;
	std::unordered_map<SSA::Instruction*, Node> nodes;
public:
	InterferenceGraph(std::vector<SSA::Instruction*> instructions);
	void addEdge(SSA::Instruction* x, SSA::Instruction* y);
	std::unordered_map<SSA::Instruction*, Node> getNodes() const;
};

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
	InterferenceGraph buildInterferenceGraph() const;
};

#endif /* INCLUDE_REGALLOCSTRUCTS_H_ */
