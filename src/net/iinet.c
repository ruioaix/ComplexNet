#include "iinet.h"
#include "error.h"
#include "mt_random.h"
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

struct iiNet *create_iiNet(const struct iiLineFile * const file) {
	int maxId=file->i1Max>file->i2Max?file->i1Max:file->i2Max;
	int minId=file->i1Min<file->i2Min?file->i1Min:file->i2Min;
	long linesNum=file->linesNum;
	struct iiLine *lines=file->lines;

	long *count=calloc(maxId+1, sizeof(long));
	assert(count!=NULL);

	long i;
	for(i=0; i<linesNum; ++i) {
		++count[lines[i].i1];
		++count[lines[i].i2];
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
		int i1 =lines[i].i1;
		int i2 =lines[i].i2;
		edges[i1][temp_count[i1]++]=i2;
		edges[i2][temp_count[i2]++]=i1;
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
	printf("build net:\tMax: %d, Min: %d, idNum: %d, edgesNum: %ld, countMax: %ld, countMin: %ld\n", maxId, minId, idNum, linesNum, countMax, countMin); fflush(stdout);
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
			//if (sign) printf("left: %d\t%d\t%d\t%ld\n", *lNum, i, id, net->count[id]);
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
				apsp[i][j] = INT_MAX;
			}
		}
	}
	for (i=0; i<net->maxId + 1; ++i) {
		printf("%.2f%%\r", i*100.0/net->maxId);fflush(stdout);
		for (j=0; j<net->maxId + 1; ++j) {
			for (k=0; k<net->maxId + 1; ++k) {
				int havek = apsp[i][k] + apsp[k][j];
				apsp[i][j] = havek < apsp[i][j] ? havek : apsp[i][j];
			}
		}
	}
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
