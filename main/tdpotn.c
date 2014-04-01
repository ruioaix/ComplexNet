#include "generatenet.h"
#include "iinet.h"
#include "error.h"
#include "sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

static void print_time(void) {
	time_t t=time(NULL); 
	printf("%s", ctime(&t)); 
	fflush(stdout);
}

static void get_all_degree(int *sp, int N, int **alld, int *alldNum, double **p_alld, double alpha) {
	int i;
	int *ddis = calloc(N, sizeof(int));

	for (i=0; i<N; ++i) {
		if (sp[i] > 0) {
			ddis[sp[i]]++;
		}
	}
	*alld = malloc(N*sizeof(int));
	*alldNum = 0;
	for (i=0; i<N; ++i) {
		if (ddis[i]) {
			(*alld)[(*alldNum)++] = i;
		}
	}
	free(ddis);

	*p_alld = malloc((*alldNum)*sizeof(double));
	for (i=0; i<*alldNum; ++i) {
		(*p_alld)[i] = pow((*alld)[i], 0-alpha);
	}
	double total = 0;
	for (i=0; i<*alldNum; ++i) {
		total += (*p_alld)[i];
	}
	for (i=0; i<*alldNum; ++i) {
		(*p_alld)[i] /= total;
	}
}

static int *set_choose(int alldNum, int *alld, double *aveDegreeN, int *accuracy) {
	int *chooseN = malloc((*accuracy)*sizeof(int));
	int i,j;
	int begin;
	int end = 0;
	for (i=0; i<alldNum; ++i) {
		begin = end;
		end += (int)(aveDegreeN[i]*(*accuracy));
		for (j=begin; j<end; ++j) {
			chooseN[j] = alld[i];
		}
	}
	*accuracy = end;
	return chooseN;
}

int main (int argc, char **argv) {
	print_time();

	int L = 10;
	double alpha = 1.0;
	enum CICLENET cc = non_cycle;
	struct iiLineFile *file = generateNet_2D(L, cc);
	//struct iiLineFile *file = generateNet_1D(L, cc);

	struct iiNet *net = create_iiNet(file);
	//the point 0 can get all kinds of degree in both cycle or non_cycle net.
	int *sp = shortestpath_1A_iiNet(net, 0);
	int *alld, alldNum;
	double *p_alld;
	get_all_degree(sp, net->maxId + 1, &alld, &alldNum, &p_alld, alpha);
	int accuracy = alldNum * 10000;
	int *choose = set_choose(alldNum, alld, p_alld, &accuracy);


//	int i;
//	for (i=0; i<alldNum; ++i) {
//		printf("%d\t%f\n", i, p_alld[i]);
//	}
//	for (i=0; i<accuracy; ++i) {
//		printf("%d\t%d\n", i, choose[i]);
//	}


	
	



	free(p_alld);
	free(choose);
	free(alld);
	free(sp);
	free_iiNet(net);
	free_iiLineFile(file);
	print_time();
	return 0;
}
