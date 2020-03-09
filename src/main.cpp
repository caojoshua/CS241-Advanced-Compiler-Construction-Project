/*
 * main.cpp
 * Author: Joshua Cao
 */

#include "Parser.h"
#include "SSA.h"
#include "SSAtoGraphML.h"

int main(int argc, char* argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		Parser parser(argv[i]);
		SSA::Program SSA = parser.parse();
		GraphML::SSAtoGraphML(SSA, argv[i]);
	}	

	return 0;
}
