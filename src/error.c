#include "error.h"
#include <stddef.h> //for NULL
#include <stdlib.h> //for exit & EXIT_FAILURE

void fileError(FILE *fp, const char * const filename)
{
	if (fp==NULL) {
		perror(filename);
		exit(EXIT_FAILURE);
	}
}

void memError(void *p, const char * const errormsg)
{
	if (p==NULL) {
		fprintf(stderr, "\nmemory error: %s\n" , errormsg);
		exit(EXIT_FAILURE);
	}
}

void isError(const char * const errormsg)
{
	fprintf(stderr, "[ERROR]:\n\t%s.\n", errormsg);
	exit(EXIT_FAILURE);
}
