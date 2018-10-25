#include "token.h"

int tokenise(char* filename, char* mytoks[])
{
	char *t;
	int i = 0;
	char delim = '\n';
	
	t = strtok(filename, &delim);
	mytoks[i] = t;

	while(t != NULL)
	{
		++i;
		if (i >= MAX_NUM_TOKENS)
		{
			i = -1;
			break;
		}

		t = strtok(NULL, &delim);
		mytoks[i] = t;
	}
	
	return i;
}
