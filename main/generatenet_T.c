#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "mt_random.h"
#include "iilinefile.h"
#include "iinet.h"

int INITVALUE = 100000;
int STEP = 10000;

void print_time(void) {
	time_t t=time(NULL); 
	printf("%s\n", ctime(&t)); 
	fflush(stdout);
}

int **generate_initNet(int N, int m0, char *initNetName) {
	FILE *fp = fopen(initNetName, "w");
	fileError(fp, initNetName);

	int **NNum = malloc(N * sizeof(void *));

	int i,j,k;
	for (i=0; i<N; ++i) {
		NNum[i] = malloc(INITVALUE * sizeof(int));
		int begin = m0*i;
		int end = m0*(i+1);
		for (j=begin; j<end; ++j) {
			NNum[i][j-begin] = j;
			for (k=j+1; k<end; ++k) {
				fprintf(fp,"%d\t%d\n", j, k);
			}
		}
	}
	fclose(fp);
	return NNum;
}

int *generate_AN(int N, int max, char *ANName) {
	FILE *fp = fopen(ANName, "w");
	fileError(fp, ANName);

	int *AN = malloc(N * sizeof(int));

	int i;
	for (i=0; i<N; ++i) {
		int aa = (int)(genrand_int31()%max + 1);
		fprintf(fp, "%d\n", aa);
		AN[i] = aa;
	}
	fclose(fp);
	return AN;
}

int main(int argc, char **argv)
{
	print_time();
	int N, m0, max, T;
	if (argc == 5) {
		char *p;
		N = strtol(argv[1], &p, 10);
		m0 = strtol(argv[2], &p, 10);
		max = strtol(argv[3], &p, 10);
		T = strtol(argv[4], &p, 10);
	}
	else {
		isError("wrong args: ./generatenet_T N m0 max T");
	}

	char *initNetName = "initNet";
	int **NNum = generate_initNet(N, m0, initNetName);
	char *ANName = "AN";
	int *AN = generate_AN(N, max, ANName);

	struct iiLineFile *netlf = create_iiLineFile(initNetName);
	struct iiNet *net = create_iiNet(netlf);
	free_iiLineFile(netlf);


	print_time();
	return 0;
}
