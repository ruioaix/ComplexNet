#include "iinetd.h"
#include "error.h"
#include "mt_random.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

void free_iiNetD(struct iiNetD *net) {
	free(net->count);
	int i=0;
	for(i=0; i<net->maxId+1; ++i) {
		free(net->to[i]);
	}
	free(net->to);
	free(net);
}

struct iiNetD *create_iiNetD(const struct iiLineFile * const file) {
	int maxId=file->i1Max>file->i2Max?file->i1Max:file->i2Max;
	int minId=file->i1Min<file->i2Min?file->i1Min:file->i2Min;
	long linesNum=file->linesNum;
	struct iiLine *lines=file->lines;

	long *count=calloc(maxId+1, sizeof(long));
	assert(count!=NULL);

	long *temp_count=calloc(maxId+1, sizeof(long));
	assert(temp_count!=NULL);

	long i;
	for(i=0; i<linesNum; ++i) {
		++count[lines[i].i1];
		++temp_count[lines[i].i2];
		++temp_count[lines[i].i1];
	}
	int j;
	int idNum=0;
	for(j=0; j<maxId+1; ++j) {
		if (temp_count[j]>0) {
			++idNum;
		}
	}

	int **to=malloc((maxId+1)*sizeof(void *));
	assert(to!=NULL);
	long countMax=count[0];
	long countMin = count[0];
	for(i=0; i<maxId+1; ++i) {
		countMax = countMax > count[i] ? countMax : count[i];
		countMin = countMin < count[i] ? countMin : count[i];
		if (count[i]!=0) {
			to[i]=malloc(count[i]*sizeof(int));
			assert(to[i]!=NULL);
		}
		else {
			to[i] = NULL;
		}
	}

	memset(temp_count, 0, (maxId+1)*sizeof(long));
	for(i=0; i<linesNum; ++i) {
		int id_from=lines[i].i1;
		int id_to=lines[i].i2;
		to[id_from][temp_count[id_from]++]=id_to;
	}
	free(temp_count);

	struct iiNetD *net=malloc(sizeof(struct iiNetD));
	assert(net != NULL);
	net->maxId=maxId;
	net->minId=minId;
	net->linesNum=linesNum;
	net->idNum=idNum;
	net->countMax=countMax;
	net->countMin=countMin;
	net->count=count;
	net->to=to;
	printf("create direct net:\n\tMax: %d, Min: %d, idNum: %d, linesNum: %ld, countMax: %ld, countMin: %ld\n", maxId, minId, idNum, linesNum, countMax, countMin); fflush(stdout);
	return net;
}

void verify_iiNetD(struct iiNetD *net) {
	long i;
	int j;
	int *place = malloc((net->maxId+1)*sizeof(int));
	FILE *fp = fopen("data/duplicatePairsinDirectNet", "w");
	fileError(fp, "data/duplicatePairsinDirectNet");
	FILE *fp2 = fopen("data/NoDuplicatePairsNetFile", "w");
	fileError(fp2, "data/NoDuplicatePairsNetFile");
	fprintf(fp, "the following pairs are duplicate in the net file\n");
	char sign=0;
	for (j=0; j<net->maxId; ++j) {
		if (net->count[j]) {
			memset(place, 0, (net->maxId+1)*sizeof(int));
			for (i=0; i<net->count[j]; ++i) {
				int origin = net->to[j][i];
				int next = place[origin];
				if (next) {
					fprintf(fp, "%d\t%d\n", j, next);
					sign=1;
				}
				else {
					place[origin]=1;
					fprintf(fp2, "%d\t%d\n", j,origin);
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
		isError("the file has duplicate pairs, you can check data/duplicatePairsinDirectNet.\nwe generate a net file named data/NoDuplicatePairsNetFile which doesn't contain any duplicate pairsr.\nyou should use this file instead the origin wrong one.\n");
	}
	else {
		printf("verify_iiNetD: perfect network.\n");
	}
}

static void shortestpath_core_iiNetD(int *sp, int **left, int **right, int *lNum, int *rNum, struct iiNetD *net, int *STEP_END, int i2) {
	int i,j;
	int STEP = 0;
	while (*lNum && STEP != *STEP_END) {
		++STEP;
		*rNum = 0;
		
		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<net->count[id]; ++j) {
				int neigh = net->to[id][j];
				if (neigh == i2) {
					*STEP_END = STEP;
					return;
				}
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

int shortestpath_11_iiNetD(struct iiNetD *net, int i1, int i2) {
	int *sp = calloc(net->maxId + 1, sizeof(int));
	int *left = malloc((net->maxId + 1)*sizeof(int));
	int *right = malloc((net->maxId + 1)*sizeof(int));
	int lNum, rNum;
	lNum = 1;
	left[0] = i1;
	sp[i1] = -1;
	int STEP_END = -1;
	shortestpath_core_iiNetD(sp, &left, &right, &lNum, &rNum, net, &STEP_END, i2);
	free(left);
	free(right);
	free(sp);
	return STEP_END;	
}

int *shortestpath_1A_iiNetD(struct iiNetD *net, int originId) {
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
	shortestpath_core_iiNetD(sp, &left, &right, &lNum, &rNum, net, &STEP_END, -1);
	free(left);
	free(right);
	return sp;	
}

int *shortestpath_1A_S_iiNetD(struct iiNetD *net, int originId, int step, int *Num) {
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
	shortestpath_core_iiNetD(sp, &left, &right, &lNum, &rNum, net, &STEP_END, -1);
	free(sp);
	free(right);
	*Num = lNum;
	return left;	
}

int *get_ALLSP_iiNetD(struct iiNetD *net) {
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
		shortestpath_core_iiNetD(sp, &left, &right, &lNum, &rNum, net, &STEP_END, -1);
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
