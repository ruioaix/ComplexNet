#include "iidnet.h"
#include "base.h"
#include "mtprand.h"
#include "sort.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

void free_iidNet(struct iidNet *net) {
	if (!net) return;
	int i=0;
	for(i=0; i<net->maxId+1; ++i) {
		if (net->count[i]>0) {
			free(net->edges[i]);
			free(net->d[i]);
		}
	}
	free(net->count);
	free(net->edges);
	free(net->d);
	free(net);
}

struct iidNet *create_iidNet(const struct LineFile * const lf) {
	if (lf == NULL) return NULL;
	if (lf->i1 == NULL || lf->i2 == NULL || lf->d1 == NULL) return NULL;

	long linesNum=lf->linesNum;
	int *i1 = lf->i1;
	int *i2 = lf->i2;
	double *d1 = lf->d1;

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
	double **d=malloc((maxId+1)*sizeof(void *));
	assert(d !=NULL);

	long countMax=0;
	long countMin=LONG_MAX;
	for(i=0; i<maxId+1; ++i) {
		countMax = countMax > count[i] ? countMax : count[i];
		countMin = countMin < count[i] ? countMin : count[i];
		if (count[i]!=0) {
			edges[i]=malloc(count[i]*sizeof(int));
			assert(edges[i]!=NULL);
			d[i]=malloc(count[i]*sizeof(double));
			assert(d[i]!=NULL);
		}
		else {
			edges[i] = NULL;
			d[i] = NULL;
		}
	}

	long *temp_count=calloc(maxId+1, sizeof(long));
	assert(temp_count!=NULL);

	for(i=0; i<linesNum; ++i) {
		int ii1 = i1[i];
		int ii2 = i2[i];
		edges[ii1][temp_count[ii1]]=ii2;
		d[ii1][temp_count[ii1]]=d1[i];
		++temp_count[ii1];
		edges[ii2][temp_count[ii2]]=ii1;
		d[ii2][temp_count[ii2]]=d1[i];
		++temp_count[ii2];
	}
	free(temp_count);

	struct iidNet *net = malloc(sizeof(struct iidNet));
	assert(net != NULL);
	net->maxId=maxId;
	net->minId=minId;
	net->edgesNum=linesNum;
	net->idNum=idNum;
	net->countMax=countMax;
	net->countMin=countMin;
	net->count=count;
	net->edges=edges;
	net->d = d;
	printf("create %s iidnet =>> Max: %d, Min: %d, idNum: %d, edgesNum: %ld, countMax: %ld, countMin: %ld\n", lf->filename, maxId, minId, idNum, linesNum, countMax, countMin); fflush(stdout);
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
					fprintf(fp, "%d, %d, %.17f\n", i, net->edges[i][j], net->d[i][j]);
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
			qsort_di_desc(net->d[i], 0, net->count[i] - 1, net->edges[i]);
		}
	}
}

void sort_asc_iidNet(struct iidNet *net) {
	int i;
	for (i=0; i<net->maxId + 1; ++i) {
		if (net->count[i]) {
			qsort_di_asc(net->d[i], 0, net->count[i] - 1, net->edges[i]);
		}
	}
}
