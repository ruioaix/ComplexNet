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

void delete_link_iiNet(struct iiNet *net, int id, int neigh) {
}

long *degree_distribution_iiNet(struct iiNet *net) {
	long *cd = calloc(net->countMax + 1, sizeof(long));
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		cd[net->count[i]]++;
	}
	return cd;
}

void verify_duplicatePairs_iiNet(struct iiNet *net) {
	long i;
	int j,k;
	int *place = malloc((net->maxId+1)*sizeof(int));
	for (k=0; k<net->maxId + 1; ++k) {
		place[k] = -1;
	}
	long dpairsNum=0;
	for (j=0; j<net->maxId+1; ++j) {
		for (i=0; i < net->count[j]; ++i) {
			int neigh = net->edges[j][i];
			if (place[neigh] == -1) {
				place[neigh] = 1;
			}
			else {
				printf("duplicate pairs %ld:\t%d\t%d\n", ++dpairsNum, j, neigh);
			}
		}
		for (i = 0; i < net->count[j]; ++i) {
			int neigh = net->edges[j][i];
			place[neigh] = -1;
		}
	}
	free(place);
	if (!dpairsNum) {
		printf("verify duplicatePairs iiNet =>> no duplicate pairs.\n");
	}
	fflush(stdout);
}

static int extract_backbone_iiNet(int nid, struct iiNet *net, char *fg, int *left, int *right) {
	if (fg[nid] == 1) isError("extract_backbone_iiNet");
	int lN = 0, rN = 0;
	left[lN++] = nid;
	fg[nid] = 1;
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
					++conn;
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

//if net->count[nid] == 0, then I presume nid node is not existed.
int robust_iiNet(struct iiNet *net) {
	int N = net->idNum;
	int maxru = 0;
	int already = 0;

	char *fg = calloc(net->maxId + 1, sizeof(char));
	int *left = malloc((net->maxId + 1) * sizeof(int));
	int *right = malloc((net->maxId + 1) * sizeof(int));

	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		if (fg[i] == 0 && net->count[i]) {
			int conn = extract_backbone_iiNet(i, net, fg, left, right);
			already += conn;
			maxru = imax(conn, maxru);
			if (maxru >= N-already) break;
		}
	}

	free(fg);
	free(left);
	free(right);
	return maxru;
}

void verify_fullyConnected_iiNet(struct iiNet *net) {
	char *fg = calloc(net->maxId + 1, sizeof(char));
	int *left = malloc((net->maxId + 1) * sizeof(int));
	int *right = malloc((net->maxId + 1) * sizeof(int));
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		if (net->count[i]) break;
	}
	int conn = extract_backbone_iiNet(i, net, fg, left, right);
	free(fg);
	free(left);
	free(right);
	if (conn != net->idNum) {
		printf("verily fullyConnected iinet =>> not connectedness.\n");
	}
	else {
		printf("verily fullyConnected iinet =>> connectedness.\n");
	}
	fflush(stdout);
}
