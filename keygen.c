
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.c"



int main(int argc, char *argv[])
{
	// Inits random generator
	srand(time(0));

	// Checks parameters count
	if (argc != 2) {
		fprintf(stderr, "\tUsage: keygen [key length]\n");
		return 1;
	}

	// Checks key size parameter
	int keylen = atoi(argv[1]);
	if (keylen < 1) {
		fprintf(stderr, "Error: [key length] < 1\n");
		return 1;
	}

	// Allocates & generates the key
	char *key = malloc(keylen+1);
	for(int i=0;i < keylen;i++) key[i] = getRandLetter();
	key[keylen] = 0;

	// Prints the key to STDOUT
	printf("%s\n", key);
	free(key);
	return 0;
}


