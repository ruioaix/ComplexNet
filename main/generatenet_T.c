#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "mt_random.h"

void print_time(void) {
	time_t t=time(NULL); 
	printf("%s\n", ctime(&t)); 
	fflush(stdout);
}


int main(int argc, char **argv)
{
	print_time();
	int N, max;
	if (argc == 3) {
		char *p;
		N = strtol(argv[1], &p, 10);
		max = strtol(argv[2], &p, 10);
	}
	else {
		isError("wrong args: ./generatenet_initNet N max");
	}

	FILE *fp = fopen("AN", "w");
	fileError(fp, "AN");

	int i;
	for (i=0; i<N; ++i) {
		fprintf(fp, "%d\n", (int)(genrand_int31()%max + 1));
	}
	fclose(fp);

	print_time();
	return 0;
}
