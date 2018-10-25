#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_NUM_TOKENS 100

int tokenise(char cmdline[], char* mytoks[]);