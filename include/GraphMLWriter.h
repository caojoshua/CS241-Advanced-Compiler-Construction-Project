/*
 * SSAtoGraphML.h
 * Author: Joshua Cao
 */

#ifndef SSATOGRAPHML_H
#define SSATOGRAPHML_H

#include "SSA.h"
#include "RegAllocStructs.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <utility>

extern std::string currFileName;

namespace GraphML
{

	static const std::string OUT_DIR = "graphml/";
	static const std::string EXTENSION = ".graphml";
	static const std::string TESTCASE_DIR = "testcases/";

	static const char* const ERR = "IO error";

	static const char* const HEADER =
		"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
		"<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:java=\"http://www.yworks.com/xml/yfiles-common/1.0/java\" xmlns:sys=\"http://www.yworks.com/xml/yfiles-common/markup/primitives/2.0\" xmlns:x=\"http://www.yworks.com/xml/yfiles-common/markup/2.0\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:y=\"http://www.yworks.com/xml/graphml\" xmlns:yed=\"http://www.yworks.com/xml/yed/3\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://www.yworks.com/xml/schema/graphml/1.1/ygraphml.xsd\">\n"
		"	<key for=\"node\" id=\"d0\" yfiles.type=\"nodegraphics\"/>\n"
		"	<graph id=\"G\" edgedefault=\"directed\">\n";

	static const char* const GRAPH_HEADER =
		"		<node id=\"{graph_id}\" yfiles.foldertype=\"group\">\n"
		"			<data key=\"d0\">\n"
		"				<y:ProxyAutoBoundsNode>\n"
		"					<y:Realizers active=\"0\">\n"
		"						<y:GroupNode>\n"
		"							<y:NodeLabel>\"{graph_id}\"</y:NodeLabel>\n"
		"						</y:GroupNode>\n"
		"					</y:Realizers>\n"
		"				</y:ProxyAutoBoundsNode>\n"
		"			</data>\n"
		"			<graph edgedefault=\"{graph_type}\">\n";

	static const char* const NODE_HEADER =
		"				<node id=\"{node_id}\">\n"
		"					<data key=\"d0\">\n"
		"						<y:ShapeNode>\n"
		"							<y:NodeLabel alignment=\"left\" autoSizePolicy=\"content\">";

	static const char* const NODE_FOOTER =
		"							</y:NodeLabel>\n"
		"							<y:BorderStyle color=\"#000000\" type=\"line\" width=\"1.0\"/>\n"
		"						</y:ShapeNode>\n"
		"					</data>\n"
		"				</node>\n";

	static const char* const EDGE =
		"				<edge id=\"{id}\" source=\"{from}\" target=\"{to}\">\n"
		"				</edge>\n";

	static const char* const GRAPH_FOOTER =
		"			</graph>\n"
		"		</node>\n";

	static const char* const FOOTER =
		"	</graph>\n"
		"</graphml>\n";

	std::ofstream getFile(char const* subdir, char const* footer = "");

	void writeSSAEdge(std::ofstream& f, std::map<SSA::BasicBlock*, std::string>& BBtoNodeId,
					SSA::BasicBlock* from, SSA::BasicBlock* to, int& edgeId);
	void writeSSAFunc(std::ofstream&, SSA::Function* func);

	/**
	 * @param ssa SSA IR to be outputted
	 * @param s write output to graphml/{s}
	 * output SSA IR in GraphML format, viewable in yEd GUI: https://www.yworks.com/products/yed
	 */
	void SSAtoGraphML(SSA::Module* module, char const* subdir);

	void InterferenceGraphToGraphML(InterferenceGraph graph, char const* subdir, char const* footer = "");

};

#endif
