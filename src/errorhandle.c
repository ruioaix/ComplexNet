#include "../inc/errorhandle.h"

void fileError(FILE *fp, char *filename)
{
	if (fp==NULL) {
		perror(filename);
		exit(EXIT_FAILURE);
	}
}

void memError(void *p, char *errormsg)
{
	if (p==NULL) {
		fprintf(stderr, "\nmemory error: %s\n" , errormsg);
		exit(EXIT_FAILURE);
	}
}

void isError(char *errormsg)
{
	fprintf(stderr, "error: %s\n", errormsg);
	exit(EXIT_FAILURE);
}
