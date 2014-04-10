#include "common.h"

#include <stdio.h>

/********************************************************************************************************/
#include <time.h>
void print_time(void) {
	time_t t=time(NULL); 
	printf("%s", ctime(&t)); 
	fflush(stdout);
}
/********************************************************************************************************/

/********************************************************************************************************/
#include "mt_random.h"
void set_RandomSeed(void) {
	time_t t=time(NULL);
	unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);
}
/********************************************************************************************************/

/********************************************************************************************************/
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
/********************************************************************************************************/
