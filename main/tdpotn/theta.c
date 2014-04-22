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

static void insert_link_to_lf(int *id1, int *id2, int sumnline, struct LineFile *lf) {
	int i;
	for (i=0; i<sumnline; ++i) {
		lf->i1[lf->linesNum] = id1[i];
		lf->i2[lf->linesNum] = id2[i];
		lf->linesNum++;
	}
}

int main (int argc, char **argv) {
	/********************************************************************************************************/
	print_time();
	set_timeseed_MTPR();
	int L;
	double alpha;
	double theta;
	if (argc == 3) {
		char *p;
		L = strtol(argv[1], &p, 10);
		theta = strtod(argv[2], &p);
	}
	else if (argc == 1) {
		L = 50;
		theta = 1;
	}
	else {
		isError("wrong args");
	}
	/********************************************************************************************************/

	int kk;
	double optimal_alpha = -1;
	double optimal_asp = LONG_MAX;
	for (kk = 0; kk < 41; ++kk) {
		alpha = kk * 0.1;

		/************get initial net.****************************************************************************/
		struct LineFile *file = lattice2d_DS(L, CYCLE, NON_DIRECT);
		//struct LineFile *file = line1d_DS(L, CYCLE, NON_DIRECT);
		struct iiNet *net = create_iiNet(file);
		int N = net->maxId + 1;
		double limit = (double)N*5;
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
		double totalL = 0;
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
			double dsplength = pow(splength, theta);
			double tmp = totalL + dsplength;
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
				//printf("%d\tout: %d, i1: %d, i2: %d, %f, %f\n", idNum, splength, i1, i2, totalL, limit);
				id1[idNum] = i1;
				id2[idNum] = i2;
				++idNum;
				totalL += dsplength;
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
		long newLen = file->linesNum + idNum;
		int *tmp;
		tmp = realloc(file->i1, (newLen)*sizeof(int));
		assert(tmp != NULL);
		file->i1 = tmp;
		tmp = realloc(file->i2, (newLen)*sizeof(int));
		assert(tmp != NULL);
		file->i2 = tmp;
		insert_link_to_lf(id1, id2, idNum, file);
		free(id1);
		free(id2);
		free_iiNet(net);
		net = create_iiNet(file);
		free_LineFile(file);
		/********************************************************************************************************/

		/*******************get average shortest path************************************************************/
		int *dis = get_ALLSP_iiNet(net);
		double aveSP = 0;
		long spNum = 0;
		int i;
		for (i=0; i<net->maxId + 1; ++i) {
			if (dis[i]) {
				aveSP += (double)dis[i]*i;
				spNum += dis[i];
			}
		}
		aveSP /= spNum;
		if (optimal_asp < aveSP) {
			optimal_asp = aveSP;
			optimal_alpha = alpha;
		}
		printf("\nresult: %d\t%d\t%f\t%f\t%.9f\n", L, N, theta, alpha, aveSP);
		free(dis);
		free_iiNet(net);
		/********************************************************************************************************/

	}
	printf("optimal: L: %d, alpha: %f, theta: %f, optimal_asp: %f\n", L, alpha, theta, optimal_alpha);
	print_time();
	return 0;
}
