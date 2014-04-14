#include "iinet.h"
#include "base.h"
#include "mtprand.h"
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

void free_iiNet(struct iiNet *net) {
	int i=0;
	for(i=0; i<net->maxId+1; ++i) {
		if (net->count[i]>0) {
			free(net->edges[i]);
		}
	}
	free(net->count);
	free(net->edges);
	free(net);
}

struct iiNet *create_iiNet(const struct LineFile * const lf) {
	//int maxId=file->i1Max>file->i2Max?file->i1Max:file->i2Max;
	//int minId=file->i1Min<file->i2Min?file->i1Min:file->i2Min;
	if (lf->i1 == NULL || lf->i2 == NULL) {
		isError("create_iiNet");
	}
	int *i1 = lf->i1;
	int *i2 = lf->i2;
	long linesNum=lf->linesNum;
	long i;
	int maxId = i1[0];
	int minId = i1[0];
	for(i=0; i<linesNum; ++i) {
		maxId = maxId > i1[i] ? maxId : i1[i];	
		maxId = maxId > i2[i] ? maxId : i2[i];	
		minId = minId < i1[i] ? minId : i1[i];
		minId = minId < i2[i] ? minId : i2[i];
	}

	long *count=calloc(maxId+1, sizeof(long));
	assert(count!=NULL);
	
	for(i=0; i<linesNum; ++i) {
		++count[i1[i]];
		++count[i2[i]];
	}

	int j;
	int idNum=0;
	for(j=0; j<maxId+1; ++j) {
		if (count[j]>0) {
			++idNum;
		}
	}

	int **edges=malloc((maxId+1)*sizeof(void *));
	assert(edges!=NULL);
	long countMax=0;
	long countMin=LONG_MAX;
	for(i=0; i<maxId+1; ++i) {
		countMax = countMax > count[i] ? countMax :count[i];
		countMin = countMin < count[i] ? countMin :count[i];
		if (count[i]!=0) {
			edges[i]=malloc(count[i]*sizeof(int));
			assert(edges[i]!=NULL);
		}
		else {
			edges[i] = NULL;
		}
	}

	long *temp_count=calloc(maxId+1, sizeof(long));
	assert(temp_count!=NULL);

	for(i=0; i<linesNum; ++i) {
		int ii1 =i1[i];
		int ii2 =i2[i];
		edges[ii1][temp_count[ii1]++]=ii2;
		edges[ii2][temp_count[ii2]++]=ii1;
	}
	free(temp_count);

	struct iiNet *net = malloc(sizeof(struct iiNet));
	assert(net != NULL);
	net->maxId=maxId;
	net->minId=minId;
	net->edgesNum=linesNum;
	net->idNum=idNum;
	net->countMax=countMax;
	net->countMin=countMin;
	net->count=count;
	net->edges=edges;
	printf("create iinet==>> Max: %d, Min: %d, idNum: %d, edgesNum: %ld, countMax: %ld, countMin: %ld\n", maxId, minId, idNum, linesNum, countMax, countMin); fflush(stdout);
	return net;
}

void *verify_iiNet(void *arg) {
	struct iiNet *net = arg;
	long i;
	int j,k;
	int *place = malloc((net->maxId+1)*sizeof(int));
	FILE *fp = fopen("data/duplicatePairsinNet", "w");
	fileError(fp, "data/duplicatePairsinNet");
	FILE *fp2 = fopen("data/NoDuplicatePairsNetFile", "w");
	fileError(fp2, "data/NoDuplicatePairsNetFile");
	fprintf(fp, "the following pairs are duplicate in the net file\n");
	char sign=0;
	for (j=0; j<net->maxId+1; ++j) {
		if (net->count[j]>0) {
			for (k=0; k<net->maxId + 1; ++k) {
				place[k] = -1;
			}
			for (i=0; i<net->count[j]; ++i) {
				int origin = net->edges[j][i];
				int next = place[origin];
				if (next == -1) {
					place[origin]=origin;
					fprintf(fp2, "%d\t%d\n", j,origin);
				}
				else {
					fprintf(fp, "%d\t%d\n", j, next);
					sign=1;
				}
			}
		}
		if (j%10000 == 0) {
			printf("%d\n", j);
			fflush(stdout);
		}
	}
	free(place);
	fclose(fp);
	fclose(fp2);
	if (sign == 1) {
		isError("the file has duplicate pairs, you can check data/duplicatePairsinNet.\nwe generate a net file named data/NoDuplicatePairsNetFile which doesn't contain any duplicate pairsr.\nyou should use this file instead the origin wrong one.\n");
	}
	else {
		printf("verify_iiNet: perfect network.\n");
	}
	return (void *)0;
}


void print_iiNet(struct iiNet *net, char *filename) {
	FILE *fp = fopen(filename, "w");
	fileError(fp, "print_iiNet");
	int i;
	long j;
	for (i=0; i<net->maxId + 1; ++i) {
		if (net->count[i] > 0) {
			for (j=0; j<net->count[i]; ++j) {
				if (i < net->edges[i][j]) {
					fprintf(fp, "%d\t%d\n", i, net->edges[i][j]);
				}
			}
		}
	}
	fclose(fp);
	printf("print_iiNet %s done. %ld lines generated.\n", filename, net->edgesNum);fflush(stdout);
}

static void shortestpath_core_iiNet(int *sp, int **left, int **right, int *lNum, int *rNum, struct iiNet *net, int *STEP_END) {
	int i,j;
	int STEP = 0;
//	int sign = 0;
//	if ((*left)[0] == 8) {
//		sign = 1;
//	}
	while (*lNum && STEP != *STEP_END) {
		++STEP;
		*rNum = 0;
		
		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			//printf("STEP: %d\t%d\t%d\t%d\t%ld\n", STEP, *lNum, i, id, net->count[id]);
			for (j=0; j<net->count[id]; ++j) {
				int neigh = net->edges[id][j];
				if (!sp[neigh]) {
					sp[neigh] = STEP;
					(*right)[(*rNum)++] = neigh;
				}
			}
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
		//printf("rNum: %d, lNum: %d, STEP_END: %d, STEP: %d\n", *rNum, *lNum, *STEP_END, STEP);
	}
	//*STEP_END = STEP;
}

int *shortestpath_1A_iiNet(struct iiNet *net, int originId) {
	if (originId<net->minId || originId>net->maxId) {
		return NULL;
	}
	int *sp = calloc(net->maxId + 1, sizeof(int));
	int *left = malloc((net->maxId + 1)*sizeof(int));
	int *right = malloc((net->maxId + 1)*sizeof(int));
	int lNum, rNum;
	lNum = 1;
	left[0] = originId;
	sp[originId] = -1;
	int STEP_END = -1;
	shortestpath_core_iiNet(sp, &left, &right, &lNum, &rNum, net, &STEP_END);
	free(left);
	free(right);
	return sp;	
}

int *get_ALLSP_iiNet(struct iiNet *net) {
	int *sp = malloc((net->maxId + 1)*sizeof(int));
	int *left = malloc((net->maxId + 1)*sizeof(int));
	int *right = malloc((net->maxId + 1)*sizeof(int));
	int lNum, rNum;

	int *distribSP = calloc(net->maxId + 1, sizeof(int));

	int i,j;
	int STEP_END = -1;
	for (i=0; i<net->maxId + 1; ++i) {
		printf("complete: %.4f%%\r", (double)i*100/(net->maxId + 1));fflush(stdout);
		lNum = 1;
		left[0] = i;
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = 0;
		}
		sp[i] = -1;
		shortestpath_core_iiNet(sp, &left, &right, &lNum, &rNum, net, &STEP_END);
		for (j=0; j<net->maxId + 1; ++j) {
			if (sp[j] > 0) {
				//printf("sp: %d\t%d\t%d\n", i, j, sp[j]);
				++distribSP[sp[j]];
			}
		}
	}

	free(left);
	free(right);
	free(sp);
	return distribSP;
}

int **shortestpath_AA_FW_iiNet(struct iiNet *net) {
	//FILE *fp = fopen("fw", "w");
	int **apsp = malloc((net->maxId + 1)*sizeof(void *));
	int i,j,k;
	for (i=0; i<net->maxId + 1; ++i) {
		apsp[i] = calloc((net->maxId + 1), sizeof(int));
	}
	for (i=0; i<net->maxId + 1; ++i) {
		for (j=0; j<net->count[i]; ++j) {
			int id = net->edges[i][j];
			apsp[i][id] = 1;
		}
	}
	for (i=0; i<net->maxId + 1; ++i) {
		for (j=0; j<net->maxId + 1; ++j) {
			if (!apsp[i][j]) {
				apsp[i][j] = 2*(net->maxId + 1);
			}
			else if (i<j){
				//fprintf(fp, "%d\t%d\t%d\n", i, j, apsp[i][j]);
			}
			if (i==j) {
				apsp[i][j] = 0;
			}
			//fprintf(fp, "%d\t%d\t%d\n", i, j, apsp[i][j]);
		}
	}
	for (k=0; k<net->maxId + 1; ++k) {
		printf("%.2f%%\r", k*100.0/net->maxId);fflush(stdout);
		for (i=0; i<net->maxId + 1; ++i) {
			for (j=0; j<net->maxId + 1; ++j) {
				int havek = apsp[i][k] + apsp[k][j];
				apsp[i][j] = havek < apsp[i][j] ? havek : apsp[i][j];
			}
		}
		//printf("%d\t%d\t%d\n", apsp[7][0], apsp[8][8], apsp[7][8]);
	}
	printf("\n");fflush(stdout);
	return apsp;
}

int *shortestpath_1A_S_iiNet(struct iiNet *net, int originId, int step, int *Num) {
	if (originId<net->minId || originId>net->maxId) {
		return NULL;
	}
	int *sp = calloc(net->maxId + 1, sizeof(int));
	int *left = malloc((net->maxId + 1)*sizeof(int));
	int *right = malloc((net->maxId + 1)*sizeof(int));
	int lNum, rNum;
	lNum = 1;
	left[0] = originId;
	sp[originId] = -1;
	int STEP_END = step;
	shortestpath_core_iiNet(sp, &left, &right, &lNum, &rNum, net, &STEP_END);
	free(sp);
	free(right);
	*Num = lNum;
	return left;	
}

static void useRate_core_iiNet(int *sp, char *use, int **left, int **right, int *lNum, int *rNum, struct iiNet *net, struct iiNet *air, int *STEP_END) {
	int i,j;
	int STEP = 0;
	while (*lNum && STEP != *STEP_END) {
		++STEP;
		*rNum = 0;
		
		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			//printf("id:%d\n", id);
			for (j=0; j<net->count[id]; ++j) {
				int neigh = net->edges[id][j];
				if (!sp[neigh]) {
					//pn q xiang yu net
					if (0 == use[id]) {
						if (-1 == use[neigh]) {
							use[neigh] = 3;
						}
						else if (4 == use[neigh]) {
							use[neigh] = 2;
							sp[neigh] = STEP;
							(*right)[(*rNum)++] = neigh;
						}
					}
					else if (1 == use[id]) {
						if (-1 == use[neigh]) {
							use[neigh] = 4;
						}
						else if (3 == use[neigh]) {
							use[neigh] = 2;
							sp[neigh] = STEP;
							(*right)[(*rNum)++] = neigh;
						}
					}
					else if (2 == use[id]) {
						use[neigh] = 2;
						sp[neigh] = STEP;
						(*right)[(*rNum)++] = neigh;
					}
				}
			}
			if(id < air->maxId + 1) {
				for (j=0; j<air->count[id]; ++j) {
					int neigh = air->edges[id][j];
					if (!sp[neigh]) {
						if (-1 == use[neigh]) {
							use[neigh] = 4;
						}
						else if (3 == use[neigh]) {
							use[neigh] = 2;
							sp[neigh] = STEP;
							(*right)[(*rNum)++] = neigh;
						}
					}
				}
			}
		}
		//static int kk = 0;
		for (j = 0; j < net->maxId + 1; ++j) {
			if (3 == use[j] || 4 == use[j]) {
				use[j] -= 3;
				sp[j] = STEP;
				(*right)[(*rNum)++] = j;
			}
			//printf("%d\t%d\n", j, use[j]);fflush(stdout);
		}
		//printf("******************************************\n");
		//if (kk++ == 4) exit(0);
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}

void get_useRate_iiNet(struct iiNet *net, struct iiNet *air, double *useRate, double *sameRate, double *cleanRate) {
	int *sp = malloc((net->maxId + 1)*sizeof(int));
	assert(sp != NULL);
	int *left = malloc((net->maxId + 1)*sizeof(int));
	assert(left != NULL);
	int *right = malloc((net->maxId + 1)*sizeof(int));
	assert(right != NULL);
	char *use = malloc((net->maxId + 1) * sizeof(char));
	assert(use != NULL);
	int lNum, rNum;

	int i,j;
	int STEP_END = -1;
	*useRate = 0;
	*sameRate = 0;
	*cleanRate = 0;
	for (i=0; i<net->maxId + 1; ++i) {
		//printf("complete: %.4f%%\r", (double)i*100/(net->maxId + 1));fflush(stdout);
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = 0;
			use[j] = -1;
		}
		sp[i] = -1;
		lNum = 0;
		for (j = 0; j < net->count[i]; ++j) {
			int to = net->edges[i][j];
			left[lNum++] = to;
			sp[to] = 1;
			use[to] = 0;
		}
		if (i < air->maxId + 1) {
			for (j = 0; j < air->count[i]; ++j) {
				int to = air->edges[i][j];
				left[lNum++] = to;
				sp[to] = 1;
				use[to] = 1;
			}
		}
		useRate_core_iiNet(sp, use, &left, &right, &lNum, &rNum, net, air, &STEP_END);
		for (j = 0; j < net->maxId + 1; ++j) {
			//printf("%d\t%d\n", j, use[j]);
			if (1 == use[j]) {
				++(*useRate);
			}
			else if (2 == use[j]) {
				++(*sameRate);
			}
			else if (0 == use[j]) {
				++(*cleanRate);
			}
			//if (sp[j]>0) {
			//	printf("%d\t%d\t%d\n", i, j, sp[j]);
			//}
		}
		//printf("*****************************\n");
	}

	free(left);
	free(right);
	free(sp);
	free(use);
	double all = (double)(net->maxId + 1)*net->maxId;
	*useRate = (*useRate)/all;
	*sameRate = (*sameRate)/all;
	*cleanRate = (*cleanRate)/all;
}
