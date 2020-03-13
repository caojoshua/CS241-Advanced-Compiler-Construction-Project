/*
 * main.cpp
 * Author: Joshua Cao
 */

#include <GraphMLWriter.h>
#include <RegAlloc.h>
#include "Parser.h"
#include "SSA.h"

std::string currFileName;

int main(int argc, char* argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		printf("compiling %s\n", argv[i]);
//		std::string currFileName = std::string(argv[i]);
		currFileName = std::string(argv[i]);
		Parser parser(argv[i]);
		SSA::IntermediateRepresentation ssa = parser.parse();
		GraphML::SSAtoGraphML(ssa, argv[i]);
		allocateRegisters(ssa);
	}	

	return 0;
}
