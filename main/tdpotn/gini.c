#include "base.h"
#include "dataset.h"
#include "iinet.h"
#include "sort.h"
#include "mtprand.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <assert.h>


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
	for (i=2; i<N; ++i) {
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
	for (i=1; i<*alldNum; ++i) {
		(*p_alld)[i] += (*p_alld)[i-1];
	}
}

static struct LineFile *create_newlf(int *id1, int *id2, int idNum) {
	struct LineFile *lf = create_LineFile(NULL);
	lf->i1 = id1;
	lf->i2 = id2;
	lf->linesNum = idNum;
	return lf;
}

int main (int argc, char **argv) {
	/********************************************************************************************************/
	print_time();
	set_timeseed_MTPR();
	int L;
	double alpha = 2;
	if (argc == 2) {
		char *p;
		L = strtol(argv[1], &p, 10);
		//alpha = strtod(argv[2], &p);
	}
	else if (argc == 1) {
		L = 10;
		//alpha = 2;
	}
	else {
		isError("wrong args");
	}
	/********************************************************************************************************/

	int kk;
	for (kk = 0; kk < 41; ++kk) {
		alpha = kk * 0.1;

		/************get initial net.****************************************************************************/
		//struct LineFile *file = lattice2d_DS(L, CYCLE, NON_DIRECT);
		struct LineFile *file = line1d_DS(L, CYCLE, NON_DIRECT);
		struct iiNet *net = create_iiNet(file);
		int N = net->maxId + 1;
		long limit = (long)N*5;
		/********************************************************************************************************/

		/**************get degree prossiblity, used to choose new links******************************************/
		//the point 0 can get all kinds of degree in both cycle or non_cycle net.
		int *sp = shortestpath_1A_iiNet(net, 0);
		int *alld, alldNum; double *p_alld;
		get_all_degree(sp, net->maxId + 1, &alld, &alldNum, &p_alld, alpha);
		free(sp);
		/********************************************************************************************************/

		/****************get new links***************************************************************************/
		int *id1 = malloc(5*N*sizeof(int));
		int *id2 = malloc(5*N*sizeof(int));
		int *hash1 = calloc((net->maxId + 1)*3, sizeof(int));
		int *hash2 = calloc((net->maxId + 1)*2, sizeof(int));
		int *hash3 = calloc((net->maxId + 1)*3, sizeof(int));
		int idNum = 0;
		int badluck = 0;
		long totalL = 0;
		while (1) {
			double chooseSPL = get_d_MTPR();
			int splength = 0;
			int i;
			for (i=0; i<alldNum; ++i) {
				if (p_alld[i] > chooseSPL) {
					splength = alld[i];
					break;
				}
			}
			long tmp = totalL + splength;
			//printf("out: %d, %ld\n", splength, tmp);
			if (tmp > limit) {
				break;
			}
			int i1 = get_i31_MTPR()%(net->maxId + 1);
			int lNum;
			int *left = shortestpath_1A_S_iiNet(net, i1, splength, &lNum);
			if (lNum > 0) {
				int random = get_i31_MTPR()%lNum;
				int i2 = left[random];
				int min = i1 < i2 ? i1 : i2;
				int max = i1 > i2 ? i1 : i2;
				if (hash1[min + 2*max] && hash2[min + max] && hash3[min*2 + max]) {
					//printf("not lucky, drop on same positon. try again.\n");
					badluck ++;
					free(left);
					continue;
				}
				hash1[min + 2*max] = 1;
				hash2[min + max] = 1;
				hash3[min*2 + max] = 1;
				//printf("%.4f%%\r", (double)totalL*100/limit);
				//printf("out: %d, i1: %d, i2: %d, %ld\n", splength, i1, i2, totalL);
				id1[idNum] = i1;
				id2[idNum] = i2;
				++idNum;
				totalL += splength;
			}
			free(left);
		}
		free(hash1);
		free(hash2);
		free(hash3);
		free(p_alld);
		free(alld);
		printf("badluck: %d, NumofAddedLinks: %d\n", badluck, idNum);
		/********************************************************************************************************/

		/*******add new links to net, get new net****************************************************************/
		struct LineFile *newlf = create_newlf(id1, id2, idNum);
		struct iiNet *newnet = create_iiNet(newlf);
		struct LineFile *together = add_LineFile(file, newlf);
		double *xe = malloc(together->linesNum * sizeof(double));
		assert(xe != NULL);
		together->d1 = xe;
		free_LineFile(newlf);
		free_LineFile(file);
		/********************************************************************************************************/

		//print_iiNet(net, "net");
		//print_iiNet(newnet, "newnet");

		/*******************get average shortest path************************************************************/
		double coupling, avesp;
		struct iidNet *XE = create_iidNet(together);
		free_LineFile(together);
		get_XE_iiNet(net, newnet, XE);
		//printf("useRate: %ld\t%f\t%f\n", limit/N, alpha, Brate);
		printf("result:\t%ld\t%f\t%f\t%f\n", limit/N, alpha, avesp, coupling);
		free_iiNet(net);
		free_iiNet(newnet);
		/********************************************************************************************************/

	}

	print_time();
	return 0;
}
