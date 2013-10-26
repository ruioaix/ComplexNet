#include "../inc/complexnet_net_snapshot.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


static struct Net_SNAPSHOT net;

struct Net_SNAPSHOT * get_Net_SNAPSHOT(void) {
	return &net;
}

void free_Net_SNAPSHOT(void) {
	int i=0;
	for(i=0; i<net.maxId+1; ++i) {
		if (net.count[i]>0) {
			free(net.edges[i]);
			free(net.status[i]);
		}
	}
	free(net.count);
	free(net.edges);
	free(net.status);
}

void create_Net_SNAPSHOT(const struct i3LineFile * const file) {
	int maxId=file->iMax;
	int minId=file->iMin;
	long linesNum=file->linesNum;
	struct i3Line *lines=file->lines;

	long *count=calloc(maxId+1, sizeof(long));
	assert(count!=NULL);

	long i;
	for(i=0; i<linesNum; ++i) {
		++count[lines[i].i1];
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
	int **status=malloc((maxId+1)*sizeof(void *));
	assert(status!=NULL);

	long countMax=0;
	for(i=0; i<maxId+1; ++i) {
		if (countMax<count[i]) {
			countMax=count[i];
		}
		if (count[i] > 0) {
			edges[i]=malloc(count[i]*sizeof(int));
			assert(edges[i]!=NULL);
			status[i]=malloc(count[i]*sizeof(int));
			assert(status[i]!=NULL);
		}
		else {
			edges[i] = NULL;
			status[i] = NULL;
		}
	}

	long *temp_count=calloc(maxId+1, sizeof(long));
	assert(temp_count!=NULL);

	for(i=0; i<linesNum; ++i) {
		int i1 =lines[i].i1;
		edges[i1][temp_count[i1]]=lines[i].i2;
		status[i1][temp_count[i1]] = lines[i].i3;
		++temp_count[i1];
	}
	free(temp_count);

	net.maxId=maxId;
	net.minId=minId;
	net.edgesNum=linesNum;
	net.vtsNum=vtsNum;
	net.countMax=countMax;
	net.count=count;
	net.edges=edges;
	net.status=status;
	printf("build net:\n\tMax: %d, Min: %d, vtsNum: %d, edgesNum: %ld, countMax: %ld\n", maxId, minId, vtsNum, linesNum, countMax); fflush(stdout);
}
