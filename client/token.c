#include "token.h"

int tokenise(char* fiename, char* mytoks[])
{
	char *t;
	int i = 0;
	
	t = strtok(cmdline, '\n');
	mytoks[i] = tk;

	while(tk != NULL)
	{
		++i;
		if (i >= MAX_NUM_TOKENS)
		{
			i = -1;
			break;
		}

		t = strtok(NULL, ';');
		mytoks[i] = tk;
	}
	
	return i;
}
