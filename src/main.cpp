/*
 * main.cpp
 * Author: Joshua Cao
 */

#include "Parser.h"

int main(int argc, char* argv[])
{
	for (int i = 1; i < argc; ++i)
	{
//		LexAnalysis::Scanner scan(argv[i]);
//		while(scan.tk != LexAnalysis::eof)
//		{
//			scan.next();
//		}

		Parser parser(argv[i]);
		parser.parse();
	}	

	return 0;
}
