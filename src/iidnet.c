#include "iidnet.h"
#include "error.h"
#include "mt_random.h"
#include "sort.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

void free_iidNet(struct iidNet *net) {
	if (!net) return;
	int i=0;
	for(i=0; i<net->maxId+1; ++i) {
		if (net->count[i]>0) {
			free(net->edges[i]);
			free(net->d3[i]);
		}
	}
	free(net->count);
	free(net->edges);
	free(net->d3);
	free(net);
}

struct iidNet *create_iidNet(const struct iidLineFile * const file) {
	if (!file) return NULL;
	int maxId=file->i1Max>file->i2Max?file->i1Max:file->i2Max;
	int minId=file->i1Min<file->i2Min?file->i1Min:file->i2Min;
	long linesNum=file->linesNum;
	struct iidLine *lines=file->lines;

	long *count=calloc(maxId+1, sizeof(long));
	assert(count!=NULL);

	long i;
	for(i=0; i<linesNum; ++i) {
		++count[lines[i].i1];
		++count[lines[i].i2];
	}
	int j;
	int vtsNum=0;
	for(j=0; j<maxId+1; ++j) {
		if (count[j]>0) {
			++vtsNum;
		}
	}

	int **edges=malloc((maxId+1)*sizeof(void *));
	assert(edges!=NULL);
	double **d3=malloc((maxId+1)*sizeof(void *));
	assert(d3 !=NULL);
	long countMax=0;
	for(i=0; i<maxId+1; ++i) {
		if (countMax<count[i]) {
			countMax=count[i];
		}
		if (count[i]!=0) {
			edges[i]=malloc(count[i]*sizeof(int));
			assert(edges[i]!=NULL);
			d3[i]=malloc(count[i]*sizeof(double));
			assert(d3[i]!=NULL);
		}
		else {
			edges[i] = NULL;
			d3[i] = NULL;
		}
	}

	long *temp_count=calloc(maxId+1, sizeof(long));
	assert(temp_count!=NULL);

	for(i=0; i<linesNum; ++i) {
		int i1 =lines[i].i1;
		int i2 =lines[i].i2;
		edges[i1][temp_count[i1]]=i2;
		d3[i1][temp_count[i1]]=lines[i].d3;
		++temp_count[i1];
		edges[i2][temp_count[i2]]=i1;
		d3[i2][temp_count[i2]]=lines[i].d3;
		++temp_count[i2];
	}
	free(temp_count);

	struct iidNet *net = malloc(sizeof(struct iidNet));
	assert(net != NULL);
	net->maxId=maxId;
	net->minId=minId;
	net->edgesNum=linesNum;
	net->vtsNum=vtsNum;
	net->countMax=countMax;
	net->count=count;
	net->edges=edges;
	net->d3 = d3;
	printf("build net:\n\tMax: %d, Min: %d, vtsNum: %d, edgesNum: %ld, countMax: %ld\n", maxId, minId, vtsNum, linesNum, countMax); fflush(stdout);
	return net;
}

void print_iidNet(struct iidNet *net, char *filename) {
	FILE *fp = fopen(filename, "w");
	fileError(fp, "print_iidNet");
	int i;
	long j;
	for (i=0; i<net->maxId + 1; ++i) {
		if (net->count[i] > 0) {
			for (j=0; j<net->count[i]; ++j) {
				if (i < net->edges[i][j]) {
					fprintf(fp, "%d, %d, %.17f\n", i, net->edges[i][j], net->d3[i][j]);
				}
			}
		}
	}
	fclose(fp);
	printf("print_iidNet %s done. %ld lines generated.\n", filename, net->edgesNum);fflush(stdout);
}

void sort_desc_iidNet(struct iidNet *net) {
	int i;
	for (i=0; i<net->maxId + 1; ++i) {
		if (net->count[i]) {
			qsort_di_desc(net->d3[i], 0, net->count[i] - 1, net->edges[i]);
		}
	}
}

void sort_asc_iidNet(struct iidNet *net) {
	int i;
	for (i=0; i<net->maxId + 1; ++i) {
		if (net->count[i]) {
			qsort_di_asc(net->d3[i], 0, net->count[i] - 1, net->edges[i]);
		}
	}
}
