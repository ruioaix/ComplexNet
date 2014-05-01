#include "tdpotn.h"
#include "base.h"
#include "dataset.h"
#include "math.h"
#include "mtprand.h"
#include <stdlib.h>
#include "spath.h"

void tdpotn_argcv(int argc, char **argv, int *L, int *seed, int *D_12, int *limitN, double *theta, double *lambda) {
	if (argc == 7) {
		char *p;
		*L = strtol(argv[1], &p, 10);
		*seed = strtol(argv[2], &p, 10);
		*D_12 = strtol(argv[3], &p, 10);
		*limitN = strtol(argv[4], &p, 10);
		*theta = strtod(argv[5], &p);
		*lambda = strtod(argv[6], &p);
	}
	else if (argc == 1) {
		*L = 50;
		*seed = 1;
		*D_12 = 1;
		*limitN = 5;
		*theta = 1;
		*lambda = 0;
	}
	else {
		isError("wrong args");
	}
}

struct LineFile * tdpotn_lf(int L, int D_12) {
	struct LineFile *file;
	if (1 == D_12) {
		file = line1d_DS(L, CYCLE, NON_DIRECT);
	}
	else if (2 == D_12) {
		file = lattice2d_DS(L, CYCLE, NON_DIRECT);
	}
	else {
		isError("wrong D_12 type");
	}
	return file;
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

struct LineFile *tdpotn_create_air(struct iiNet * net, double alpha, int limitN, double theta, double lambda) {
	if (theta < 0.5) isError("theta should be [0.5, +00)");

	int N = net->maxId + 1;
	double limit = (double)N*limitN;

	/**************get degree prossiblity, used to choose new links******************************************/
	//the point 0 can get all kinds of degree in both cycle or non_cycle net.
	int *sp = spath01_1A_iiNet(net, 0);
	int *alld, alldNum; double *p_alld;
	get_all_degree(sp, net->maxId + 1, &alld, &alldNum, &p_alld, alpha);
	free(sp);
	/********************************************************************************************************/

	/****************get new links***************************************************************************/
	int *id1 = malloc(limitN*N*sizeof(int));
	int *id2 = malloc(limitN*N*sizeof(int));
	double *weight = malloc(limitN*N*sizeof(double));
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
		if (tmp > limit) {
			break;
		}
		int i1 = get_i31_MTPR()%(net->maxId + 1);
		int lNum;
		int *left = spath01_step_1A_iiNet(net, i1, splength, &lNum);
		if (lNum > 0) {
			int random = get_i31_MTPR()%lNum;
			int i2 = left[random];
			int min = imin(i1, i2);
			int max = imax(i1, i2);
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
			id1[idNum] = i1;
			id2[idNum] = i2;
			weight[idNum] = pow(splength, lambda);
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

	/*******create a air struct LineFile ********************************************************************/
	struct LineFile *lf = create_LineFile(NULL);
	lf->i1 = id1;
	lf->i2 = id2;
	lf->d1 = weight;
	lf->linesNum = idNum;
	lf->memNum = limitN*N*sizeof(int);
	lf->filename = "air";

	return lf;
	/********************************************************************************************************/
}
