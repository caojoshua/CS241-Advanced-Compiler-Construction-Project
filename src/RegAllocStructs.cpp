/*
 * RegAllocStrucs.c
 * Author: Joshua Cao
 */

# include "RegAllocStructs.h"

InterferenceGraph::Node::Node(SSA::Instruction *i) :
		Node(-1, i)
{
}

InterferenceGraph::Node::Node(const Node &other) :
		id(other.id), edges(other.edges), instruction(other.instruction)
{
}

InterferenceGraph::Node::Node(int id, SSA::Instruction *i) :
		id(id), instruction(i)
{
}

InterferenceGraph::Node* InterferenceGraph::getNode(SSA::Instruction *i)
{
	for (Node &n : nodes)
	{
		if (n.instruction == i)
		{
			return &n;
		}
	}
	return nullptr;
}

void InterferenceGraph::clearMatrixEdges(Node n)
{
	for (int i = 0; i < adjacencyMatrix.size(); ++i)
	{
		adjacencyMatrix[n.id][i] = false;
		adjacencyMatrix[i][n.id] = false;
	}
}

InterferenceGraph::Node* InterferenceGraph::popNode(int k)
{
	for (std::list<Node>::iterator iter = nodes.begin(); iter != nodes.end();
			++iter)
	{
		Node *n = new Node(*iter);
		int sum = 0;
		for (int i = 0; i < adjacencyMatrix.size(); ++i)
		{
			if (adjacencyMatrix[n->id][i] && n->id != i)
			{
				sum += 1;
			}
		}
		if (sum < k)
		{
			clearMatrixEdges(*n);
			nodes.erase(iter);
			return n;
		}
	}
	return nullptr;
}

std::list<InterferenceGraph::Node>::iterator InterferenceGraph::spillNode()
{
	// TODO: heurisitc
	for (std::list<Node>::iterator iter = nodes.begin(); iter != nodes.end();
			++iter)
	{
		return iter;
	}
	return nodes.end();
}

InterferenceGraph::InterferenceGraph(
		std::vector<SSA::Instruction*> instructions, SSA::Function* f) : f(f)
{
	int numNodes = instructions.size();
	adjacencyMatrix.reserve(numNodes);
	for (int i = 0; i < numNodes; ++i)
	{
		nodes.push_back(Node(i, instructions[i]));
		adjacencyMatrix.push_back(std::vector<bool>());
		adjacencyMatrix[i].reserve(numNodes);
		for (int j = 0; j < numNodes; ++j)
		{
			adjacencyMatrix[i].push_back(false);
		}
	}
}

void InterferenceGraph::addEdge(SSA::Instruction *x, SSA::Instruction *y)
{
	Node *xNode = getNode(x);
	Node *yNode = getNode(y);
	if (xNode and yNode)
	{
		adjacencyMatrix[xNode->id][yNode->id] = true;
		adjacencyMatrix[yNode->id][xNode->id] = true;
		xNode->edges.push_back(y);
		if (!(x == y))
		yNode->edges.push_back(x);
	}
}

std::list<InterferenceGraph::Node> InterferenceGraph::getNodes() const
{
	return nodes;
}

/*
 * G. J. Chaitin
 * Register Allocation & Spilling via Graph Coloring
 *
 * additional notes:
 * 1. when popping nodes, update the edges in the adjacency matrix
 * 		- look through adjacency matrix to find it it has < k neighbors
 * 		- this is much more efficient than going through each node and updating its edge
 * 		if one exists with the current node
 * 		- no need to restore matrix since subsequent coloring only relies on nodes'
 * 		adjacency vectors
 */
void InterferenceGraph::colorGraph(int k)
{
	std::stack<Node> stack;
	std::list<Node> spillSet;

	while (!nodes.empty())
	{
		// pop a node with < k neighbors
		Node *n = popNode(k);
		while (n)
		{
			stack.push(*n);
			// TODO: adding to container should create a copy, but idk if this is correct
			// if it copies by reference, should remove following line
			delete n;
			n = popNode(k);
		}

		// if node is not empty, choose a node to spill
		if (!nodes.empty())
		{
			std::list<Node>::iterator i = spillNode();
			spillSet.push_back(*i);
			clearMatrixEdges(*i);
			nodes.erase(i);
		}
	}

	// insert spill code and reconstruct interference graph
	if (!spillSet.empty())
	{
		int offset = spillSet.front().instruction->getParent()->getParent()->getLocalVariableOffset();
		for (Node n : spillSet)
		{
			n.instruction->insertAfter(new SSA::Instruction(SSA::store,
					new SSA::ValOperand(n.instruction), new SSA::ConstOperand(offset)));
			for (SSA::BasicBlock* b : n.instruction->getParent()->getParent()->getBBs())
			{
				for (SSA::Instruction* i : b->getInstructions())
				{
					SSA::ValOperand* val = new SSA::ValOperand(n.instruction);
					// dont replace args in the newly inserted store
					// can be better by iterating from the current instruction, but more work
					if (i->containsArg(val) && i->getOpcode() != SSA::store)
					{
						SSA::Instruction* load = new SSA::Instruction(SSA::load, new SSA::ConstOperand(offset));
						i->insertBefore(load);
						i->replaceArg(val, new SSA::ValOperand(load));
					}
				}
			}
			offset -= 4;
		}
		spillSet.front().instruction->getParent()->getParent()->setLocalVariableOffset(offset);
		allocateRegisters(f);
	}

	// assign lowest possible color for each node in stack
	while (!stack.empty())
	{
		Node n = stack.top();
		stack.pop();
		int color;
		for (color = 0; color < k; ++color)
		{
			bool foundColor = true;
			for (SSA::Instruction *i : n.edges)
			{
				if (i->getReg() == color)
				{
					foundColor = false;
					break;
				}
			}
			if (foundColor)
			{
				n.instruction->setReg(color);
				break;
			}
		}
	}
}

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
	for (std::pair<int, int> &range : ranges)
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
	for (std::pair<int, int> &range : ranges)
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
		for (std::pair<int, int> otherRange : other.ranges)
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

InterferenceGraph IntervalList::buildInterferenceGraph() const
{
	std::vector<SSA::Instruction*> instructions;
	for (std::pair<SSA::Instruction*, Interval> interval : intervals)
	{
		instructions.push_back(interval.first);
	}
	InterferenceGraph graph(instructions, f);

	for (auto i = intervals.cbegin(); i != intervals.cend(); ++i)
	{
		for (auto j = i; j != intervals.cend(); ++j)
		{
			if ((*i).second.intersects((*j).second))
			{
				graph.addEdge((*i).first, (*j).first);
			}
		}
	}

	return graph;
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
		switch (o->getType())
		{
		case SSA::Operand::val:
			addRange(o->getInstruction(), from, to);
			break;
		case SSA::Operand::phi:
		case SSA::Operand::call:
			for (SSA::Operand *arg : o->getArgs())
			{
				addRange(arg, from, to);
			}
			break;
		}
	}
}

std::list<std::pair<int, int>> IntervalList::getRanges(
		SSA::Instruction *i) const
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
	} else
	{
		intervals[i] = Interval(from, from);
	}
}
