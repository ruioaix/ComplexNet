#include "common.h"
#include "generateds.h"
#include "iinet.h"
#include "sort.h"
#include "mt_random.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>


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

static void insert_link_to_lf(int *id1, int *id2, int sumnline, struct iiLineFile *lf) {
	int i;
	for (i=0; i<sumnline; ++i) {
		lf->lines[lf->linesNum].i1 = id1[i];
		lf->lines[lf->linesNum].i2 = id2[i];
		lf->i1Max = lf->i1Max > id1[i]?lf->i1Max:id1[i];
		lf->i1Min = lf->i1Min < id1[i]?lf->i1Min:id1[i];
		lf->i2Max = lf->i2Max > id2[i]?lf->i2Max:id2[i];
		lf->i2Min = lf->i2Min < id2[i]?lf->i2Min:id2[i];
		lf->linesNum++;
	}
}

int main (int argc, char **argv) {
	/********************************************************************************************************/
	print_time();
	set_RandomSeed();
	int L;
	double alpha;
	if (argc == 3) {
		char *p;
		L = strtol(argv[1], &p, 10);
		//alpha = strtod(argv[2], &p);
	}
	else if (argc == 1) {
		L = 50;
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
	struct iiLineFile *file = generate_2DLattice(L, cycle, non_direct);
	//struct iiLineFile *file = generate_1DLine(L, cycle, non_direct);
	struct iiNet *net = create_iiNet(file);
	int N = net->maxId + 1;
	long limit = (long)N*10;
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
		double chooseSPL = genrand_real3();
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
		int i1 = genrand_int31()%(net->maxId + 1);
		int lNum;
		int *left = shortestpath_1A_S_iiNet(net, i1, splength, &lNum);
		if (lNum > 0) {
			int random = genrand_int31()%lNum;
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
	long newLen = file->linesNum + idNum;
	struct iiLine * tmp = realloc(file->lines, (newLen)*sizeof(struct iiLine));
	assert(tmp != NULL);
	file->lines = tmp;
	insert_link_to_lf(id1, id2, idNum, file);
	free(id1);
	free(id2);
	free_iiNet(net);
	net = create_iiNet(file);
	free_iiLineFile(file);
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
	printf("\nresult: %d\t%d\t%f\t%.9f\n", L, N, alpha, aveSP);
	free(dis);
	free_iiNet(net);
	/********************************************************************************************************/

	}
	print_time();
	return 0;
}
