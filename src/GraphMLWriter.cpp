/*
 * SSAtoGraphML.cpp
 * Author: Joshua Cao
 */

#include <GraphMLWriter.h>

std::ofstream GraphML::getFile(char const* subdir, char const* footer)
{
	std::string str = currFileName;
	std::size_t testcaseDirIndex = str.find(TESTCASE_DIR) + TESTCASE_DIR.length();
	str = str.substr(testcaseDirIndex);

	std::size_t fileIndex = str.find_last_of("/\\");
	str.insert(fileIndex+1, std::string(subdir));

	fileIndex = str.find_last_of("/\\");
	system(("mkdir -p " + OUT_DIR + str.substr(0, fileIndex)).c_str());

	std::size_t extensionIndex = str.find(".txt");
	std::string fname = OUT_DIR + str.substr(0, extensionIndex) + footer + EXTENSION;

	return std::ofstream(fname);
}

void GraphML::writeSSAEdge(std::ofstream& f, std::map<SSA::BasicBlock*, std::string>& BBtoNodeId,
				SSA::BasicBlock* from, SSA::BasicBlock* to, int& edgeId)
{
	if (from && to)
	{
		std::string str = std::string(EDGE);
		str.replace(str.find("{id}"), 4, "e" + std::to_string(edgeId));
		str.replace(str.find("{from}"), 6, BBtoNodeId[from]);
		str.replace(str.find("{to}"), 4, BBtoNodeId[to]);
		++edgeId;
		f << str;
	}
}

void GraphML::writeSSAFunc(std::ofstream& f, SSA::Function* func)
{
	std::string funcName = func->getName();
	std::map<SSA::BasicBlock*, std::string> BBtoNodeId;
	int bbId = 0;
	std::string funcHeader = GRAPH_HEADER;
	funcHeader.replace(funcHeader.find("{graph_id}"), 10, funcName);
	funcHeader.replace(funcHeader.find("{graph_id}"), 10, funcName);
	funcHeader.replace(funcHeader.find("{graph_type}"), 12, "directed");
	f << funcHeader;
	std::list<SSA::BasicBlock*> BBs = func->getBBs();
	for (SSA::BasicBlock* bb : BBs)
	{
		BBtoNodeId[bb] = funcName + std::to_string(bbId);
		std::string bbHeader = std::string(NODE_HEADER);
		bbHeader.replace(bbHeader.find("{node_id}"), 9, funcName + std::to_string(bbId));
		f << bbHeader;
		for (SSA::Instruction* instruction : bb->getInstructions())
		{
			f << std::endl << instruction->toStr();
		}
		++bbId;
		f << NODE_FOOTER;
	}
	int edgeId = 0;
	for (SSA::BasicBlock* bb : BBs)
	{
		for (SSA::BasicBlock* succ : bb->getSuccessors())
		{
			writeSSAEdge(f, BBtoNodeId, bb, succ, edgeId);
		}
		}
	f << GRAPH_FOOTER;
}

void GraphML::SSAtoGraphML(SSA::Module* module, char const* subdir)
{
	std::ofstream f = getFile(subdir);
	if (f)
	{
		f << HEADER;
		for (SSA::Function* func : module->getFuncs())
		{
			if (func->getName() != "InputNum"
					&& func->getName() != "OutputNum"
					&& func->getName() != "OutputNewLine")
			{
				writeSSAFunc(f, func);
			}
		}
		f << FOOTER;
		f.close();
	}
	else
	{
		std::cout << ERR << std::endl;
	}
}

void GraphML::InterferenceGraphToGraphML(InterferenceGraph graph, char const* subdir, char const* footer)
{
	std::ofstream f = getFile(subdir, footer);

	if (f)
	{
		f << HEADER;
		std::string graphHeader = std::string(GRAPH_HEADER);
		graphHeader.replace(graphHeader.find("{graph_type}"), 12, "undirected");
		f << graphHeader;

		std::list<InterferenceGraph::Node> nodes = graph.getNodes();
		std::unordered_map<SSA::Instruction*, int> nodeIds;
		int nodeId = 0;

		for (InterferenceGraph::Node node : nodes)
		{
			std::string nodeStr = std::string(NODE_HEADER);
			nodeStr.replace(nodeStr.find("{node_id}"), 9, std::to_string(nodeId));
			f << nodeStr << node.instruction->toStr() << NODE_FOOTER;

			nodeIds[node.instruction] = nodeId;
			++nodeId;
		}

		int edgeId = 0;
		for (InterferenceGraph::Node node : nodes)
		{
			for (SSA::Instruction* i : node.edges)
			{
				std::string edge = std::string(EDGE);
				edge.replace(edge.find("{id}"), 4, std::to_string(edgeId));
				edge.replace(edge.find("{from}"), 6, std::to_string(nodeIds[node.instruction]));
				edge.replace(edge.find("{to}"), 4, std::to_string(nodeIds[i]));
				f << edge;
				++edgeId;
			}
		}

		f << GRAPH_FOOTER << FOOTER;
		f.close();
	}
	else
	{
		std::cout << ERR << std::endl;
	}
}


