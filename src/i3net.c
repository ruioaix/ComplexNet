#include "i3net.h"
#include "base.h"
#include "mtprand.h"
#include "sort.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

void free_i3Net(struct i3Net *net) {
	print2l("%s =>> begin......\n", __func__);
	if (!net) return;
	int i=0;
	for(i=0; i<net->maxId+1; ++i) {
		if (net->count[i]>0) {
			free(net->edges[i]);
			free(net->i3[i]);
		}
	}
	free(net->count);
	free(net->edges);
	free(net->i3);
	free(net);
	print2l("%s =>> ......end.\n", __func__);
}

struct i3Net *create_i3Net(const struct LineFile * const lf) {
	print2l("%s =>> begin......\n", __func__);
	if (lf == NULL) return NULL;
	if (lf->i1 == NULL || lf->i2 == NULL || lf->i3 == NULL) return NULL;

	long linesNum=lf->linesNum;
	int *i1 = lf->i1;
	int *i2 = lf->i2;
	int *i3 = lf->i3;

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
	int **ii3 =malloc((maxId+1)*sizeof(void *));
	assert(ii3 !=NULL);

	long countMax=0;
	long countMin=LONG_MAX;
	for(i=0; i<maxId+1; ++i) {
		countMax = countMax > count[i] ? countMax : count[i];
		countMin = countMin < count[i] ? countMin : count[i];
		if (count[i]!=0) {
			edges[i]=malloc(count[i]*sizeof(int));
			assert(edges[i]!=NULL);
			ii3[i]=malloc(count[i]*sizeof(double));
			assert(ii3[i]!=NULL);
		}
		else {
			edges[i] = NULL;
			ii3[i] = NULL;
		}
	}

	long *temp_count=calloc(maxId+1, sizeof(long));
	assert(temp_count!=NULL);

	for(i=0; i<linesNum; ++i) {
		int ii1 = i1[i];
		int ii2 = i2[i];
		edges[ii1][temp_count[ii1]]=ii2;
		ii3[ii1][temp_count[ii1]]=i3[i];
		++temp_count[ii1];
		edges[ii2][temp_count[ii2]]=ii1;
		ii3[ii2][temp_count[ii2]]=i3[i];
		++temp_count[ii2];
	}
	free(temp_count);

	struct i3Net *net = malloc(sizeof(struct i3Net));
	assert(net != NULL);
	net->maxId=maxId;
	net->minId=minId;
	net->edgesNum=linesNum;
	net->idNum=idNum;
	net->countMax=countMax;
	net->countMin=countMin;
	net->count=count;
	net->edges=edges;
	net->i3 = ii3;
	print2l("%s =>> ......end.\n", __func__);
	return net;
}

void print_i3Net(struct i3Net *net, char *filename) {
	FILE *fp = fopen(filename, "w");
	fileError(fp, "print_i3Net");
	int i;
	long j;
	for (i=0; i<net->maxId + 1; ++i) {
		if (net->count[i] > 0) {
			for (j=0; j<net->count[i]; ++j) {
				if (i < net->edges[i][j]) {
					fprintf(fp, "%d\t%d\t%d\n", i, net->edges[i][j], net->i3[i][j]);
				}
			}
		}
	}
	fclose(fp);
	printf("print_i3Net %s done. %ld lines generated.\n", filename, net->edgesNum);fflush(stdout);
}
