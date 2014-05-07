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

void delete_node_iiNet(struct iiNet *net, int nid) {
	long i, j;
	if (net->count[nid] == 0) return;
	for (i = 0; i < net->count[nid]; ++i) {
		int neigh = net->edges[nid][i];
		assert(net->count[neigh] != 0);
		for (j = 0; j < net->count[neigh]; ++j) {
			if (net->edges[neigh][j] == nid) {
				net->edges[neigh][j] = net->edges[neigh][--(net->count[neigh])];
				break;
			}
		}
		if (net->count[neigh] == 0) {
			net->idNum--;
			free(net->edges[neigh]);
			net->edges[neigh] = NULL;
		}
	}
	free(net->edges[nid]);
	net->edges[nid] = NULL;
	net->count[nid] = 0;
	net->idNum--;
	//printf("delete node %d from iiNet =>> done\n", nid);
}

long *degree_distribution_iiNet(struct iiNet *net) {
	long *cd = calloc(net->countMax + 1, sizeof(long));
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		cd[net->count[i]]++;
	}
	return cd;
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




void verify_connectedness_iiNet(struct iiNet *net) {
	char *fg = calloc(net->maxId + 1, sizeof(char));
	int i;
	int *left = calloc(net->maxId + 1, sizeof(int));
	int *right = calloc(net->maxId + 1, sizeof(int));
	int lN = 0, rN = 0;
	left[lN++] = 0;
	fg[0] = 1;
	int conn = 1;
	long j;
	while(lN && conn != net->idNum) {
		rN = 0;
		for (i = 0; i < lN; ++i) {
			int id = left[i];
			for (j = 0; j < net->count[id]; ++j) {
				int neigh = net->edges[id][j];
				if (fg[neigh] == 0) {
					fg[neigh] = 1;
					conn++;
					right[rN++] = neigh;
				}
			}
		}
		int *tmp = left;
		left = right;
		right = tmp;
		lN = rN;
	}
	if (conn != net->idNum) {
		printf("verily iinet =>> not connectedness.\n");
	}
	else {
		printf("verily iinet =>> connectedness.\n");
	}
	free(fg);
	free(left);
	free(right);
}

static int extract_backbone_iiNet(struct iiNet *net, char *fg, int *left, int *right, int lN, int rN) {
	int conn = 1;
	int i;
	long j;
	while(lN) {
		rN = 0;
		for (i = 0; i < lN; ++i) {
			int id = left[i];
			for (j = 0; j < net->count[id]; ++j) {
				int neigh = net->edges[id][j];
				if (fg[neigh] == 0) {
					fg[neigh] = 1;
					conn++;
					right[rN++] = neigh;
				}
			}
		}
		int *tmp = left;
		left = right;
		right = tmp;
		lN = rN;
	}
	return conn;
}

int robust_iiNet(struct iiNet *net) {
	int N = net->idNum;
	int maxru = 0;
	int already = 0;
	char *fg = calloc(net->maxId + 1, sizeof(char));
	int i;
	int *left = calloc(net->maxId + 1, sizeof(int));
	int *right = calloc(net->maxId + 1, sizeof(int));
	int lN = 0, rN = 0;

	for (i = 0; i < net->maxId + 1; ++i) {
		if (fg[i] == 0 && net->count[i]) {
			lN = 0;
			left[lN++] = i;
			fg[i] = 1;
			int conn = extract_backbone_iiNet(net, fg, left, right, lN, rN);
			already += conn;
			maxru = imax(conn, maxru);
			if (maxru >= N-already) break;
			//printf("%d,%d,%d,%d<|||||||>", conn, maxru, already, N-already);
		}
	}

	//printf("\n");

	free(fg);
	free(left);
	free(right);
	return maxru;
}
