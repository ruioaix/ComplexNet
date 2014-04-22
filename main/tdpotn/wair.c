#include "base.h"
#include "dataset.h"
#include "iinet.h"
#include "iidnet.h"
#include "sort.h"
#include "mtprand.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <assert.h>

#define EPS 0.0000001

static void useRate_core_Net(double *sp, char *gs, int **left, int **right, int *lNum, int l2, int *rNum, struct iiNet *net, struct iidNet *air, int *STEP_END) {
	int i,j;
	int STEP = 1;
	while (*lNum && STEP != *STEP_END) {
		*rNum = 0;
		++STEP;
		
		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<net->count[id]; ++j) {
				int neigh = net->edges[id][j];
				//if gs = 1, neigh is already defined.
				if (!gs[neigh] ) {
					sp[neigh] = STEP;
					(*right)[(*rNum)++] = neigh;
					gs[neigh] = 1;
				}
			}
			if(id < air->maxId + 1) {
				for (j=0; j<air->count[id]; ++j) {
					int neigh = air->edges[id][j];
					if ( !gs[neigh] ) {
						sp[neigh] = STEP + air->d[id][j];
						(*right)[(*rNum)++] = neigh;
					}
				}
			}
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;

	}
}

static void get_avesp_Net(struct iiNet *net, struct iidNet *air, double *avesp) {
	double *sp = malloc((net->maxId + 1)*sizeof(double));
	assert(sp != NULL);
	int *left = malloc((net->maxId + 1)*sizeof(int));
	assert(left != NULL);
	int *right = malloc((net->maxId + 1)*sizeof(int));
	assert(right != NULL);
	int lNum, rNum;

	char *gs = malloc((net->maxId + 1)*sizeof(char));

	*avesp = 0;
	int cc = 0;

	int i,j;
	int STEP_END = -1;
	for (i=0; i<net->maxId + 1; ++i) {
		//printf("complete: %.4f%%\r", (double)i*100/(net->maxId + 1));fflush(stdout);
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = 0.0;
			gs[j] = 0;
		}
		sp[i] = -1;
		gs[i] = -1;
		lNum = 0;
		l2 = net->maxId;
		for (j = 0; j < net->count[i]; ++j) {
			int to = net->edges[i][j];
			left[lNum++] = to;
			sp[to] = 1.0;
			gs[to] = 1;
		}
		if (i < air->maxId + 1) {
			for (j = 0; j < air->count[i]; ++j) {
				int to = air->edges[i][j];
				if (!gs[to] ) {
					if (air->d[i][j] < 2) {
						sp[to] = air->d[i][j];
						gs[to] = 1;
						left[l2--] = to;
					}
					else {
						sp[to] = air->d[i][j];
					}
				}
			}
		}
		useRate_core_Net(sp, gs, &left, &right, &lNum, l2, &rNum, net, air, &STEP_END);
		for (j = 0; j < net->maxId + 1; ++j) {
			if (sp[i] > 0) {
				*avesp += sp[i];
				++cc;
			}
		}
	}

	free(left);
	free(right);
	free(sp);
	double all = (double)(net->maxId + 1)*net->maxId;
	*avesp = (*avesp)/all;
	printf("check: %d\t%f\n", cc, all);
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

static struct LineFile *create_newlf(int *id1, int *id2, double *weight, int idNum) {
	struct LineFile *lf = init_LineFile();
	lf->i1 = id1;
	lf->i2 = id2;
	lf->d1 = weight;
	lf->linesNum = idNum;
	return lf;
}

int main (int argc, char **argv) {
	/********************************************************************************************************/
	print_time();
	set_timeseed_MTPR();
	int L;
	double alpha, lambda;
	if (argc == 3) {
		char *p;
		L = strtol(argv[1], &p, 10);
		lambda = strtod(argv[2], &p);
	}
	else if (argc == 1) {
		L = 10;
		lambda = 2;
	}
	else {
		isError("wrong args");
	}
	/********************************************************************************************************/

	int kk;
	for (kk = 0; kk < 41; ++kk) {
		alpha = kk * 0.1;

		/************get initial net.****************************************************************************/
		struct LineFile *file = lattice2d_DS(L, CYCLE, NON_DIRECT);
		//struct LineFile *file = line1d_DS(L, CYCLE, NON_DIRECT);
		struct iiNet *net = create_iiNet(file);
		free_LineFile(file);
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
		double *weight = malloc(5*N*sizeof(double));
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
				weight[idNum] = pow(splength, lambda);
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
		struct LineFile *newlf = create_newlf(id1, id2, weight, idNum);
		struct iidNet *newnet = create_iidNet(newlf);
		free_LineFile(newlf);
		/********************************************************************************************************/

		//print_iiNet(net, "net");
		//print_iiNet(newnet, "newnet");

		/*******************get average shortest path************************************************************/
		double avesp;
		get_avesp_Net(net, newnet, &avesp);
		free_iiNet(net);
		free_iidNet(newnet);
		/********************************************************************************************************/

	}

	print_time();
	return 0;
}
