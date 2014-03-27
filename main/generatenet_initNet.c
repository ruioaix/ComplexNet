#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

void print_time(void) {
	time_t t=time(NULL); 
	printf("%s\n", ctime(&t)); 
	fflush(stdout);
}


int main(int argc, char **argv)
{
	print_time();
	int N, m0;
	if (argc == 3) {
		char *p;
		N = strtol(argv[1], &p, 10);
		m0 = strtol(argv[2], &p, 10);
	}
	else {
		isError("wrong args: ./generatenet_initNet N m0");
	}

	FILE *fp = fopen("initNet", "w");
	fileError(fp, "initNet");

	int i,j,k;
	for (i=0; i<N; ++i) {
		int begin = m0*i;
		int end = m0*(i+1);
		for (j=begin; j<end; ++j) {
			for (k=j+1; k<end; ++k) {
				fprintf(fp,"%d\t%d\n", j, k);
			}
		}
	}
	fclose(fp);

	print_time();
	return 0;
}
