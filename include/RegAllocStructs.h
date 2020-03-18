/*
 * RegAllocStructs.h
 * Author: Joshua Cao
 */

#ifndef INCLUDE_REGALLOCSTRUCTS_H_
#define INCLUDE_REGALLOCSTRUCTS_H_

#include "SSA.h"
#include "RegAlloc.h"
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <stack>
#include <list>

const int NUM_REG = 6;

class InterferenceGraph
{
public:
	struct Node
	{
	public:
		Node(SSA::Instruction* i);
		Node(const Node& other);
		Node(int id, SSA::Instruction* i);
		int id;
		SSA::Instruction* instruction;
		std::list<SSA::Instruction*> edges;
	};
private:
	// edges stored in adjacency matrix and within nodes
	// G. J. Chaitin
	// Register Allocation & Spilling via Graph Coloring
	std::vector<std::vector<bool>> adjacencyMatrix;
	std::list<Node> nodes;
	SSA::Function* f;

	void clearMatrixEdges(Node n);
	Node* getNode(SSA::Instruction* i);
	Node* popNode(int k);
	std::list<Node>::iterator spillNode();
public:
	InterferenceGraph(std::vector<SSA::Instruction*> instructions, SSA::Function* f);
	void addEdge(SSA::Instruction* x, SSA::Instruction* y);
	std::list<Node> getNodes() const;
	void colorGraph(int k);
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
	SSA::Function* f;
	std::map<SSA::Instruction*, Interval> intervals;
public:
	IntervalList(SSA::Function* f) : f(f) {}
	std::list<std::pair<int, int>> getRanges(SSA::Instruction* i) const;
	void setFrom(SSA::Instruction* i, int from);
	void addRange(SSA::Instruction* i, int from, int to);
	void addRange(SSA::Operand* o, int from, int to);
	InterferenceGraph buildInterferenceGraph() const;
};

#endif /* INCLUDE_REGALLOCSTRUCTS_H_ */
