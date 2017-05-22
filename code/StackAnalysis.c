#include <stdio.h>

void registerFunction(char functionName[]) {
	printf("In function: %s\n", functionName);
}

void deregisterFunction(char functionName[]) {
	printf("Exit function: %s\n", functionName);
}


