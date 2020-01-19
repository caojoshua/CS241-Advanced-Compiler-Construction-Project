
#include "Scanner.h"

int main(int argc, char* argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		Scanner scan(argv[i]);
		while(scan.tk != eof)
		{
			scan.next();		
		}
	}	

	return 0;
}