#include "../inc/complexnet_net_pspipr.h"
#include "../inc/complexnet_sort.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


static struct Net_PSPIPR net;

struct Net_PSPIPR * get_Net_PSPIPR(void) {
	return &net;
}

void free_Net_PSPIPR(void) {
	int i=0;
	for(i=0; i<net.maxId+1; ++i) {
		if (net.count[i]>0) {
			free(net.edges[i]);
			free(net.PS[i]);
			free(net.PI[i]);
			free(net.PR[i]);
		}
	}
	free(net.count);
	free(net.edges);
	free(net.PS);
	free(net.PI);
	free(net.PR);
}

void create_Net_PSPIPR(const struct iid3LineFile * const file) {
	int maxId=file->iMax;
	int minId=file->iMin;
	long linesNum=file->linesNum;
	struct iid3Line *lines=file->lines;

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
	double **PS=malloc((maxId+1)*sizeof(void *));
	assert(PS!=NULL);
	double  **PI=malloc((maxId+1)*sizeof(void *));
	assert(PI!=NULL);
	double **PR=malloc((maxId+1)*sizeof(void *));
	assert(PR!=NULL);

	long countMax=0;
	for(i=0; i<maxId+1; ++i) {
		if (countMax<count[i]) {
			countMax=count[i];
		}
		if (count[i] > 0) {
			edges[i]=malloc(count[i]*sizeof(int));
			assert(edges[i]!=NULL);
			PS[i]=malloc(count[i]*sizeof(double));
			assert(PS[i]!=NULL);
			PI[i]=malloc(count[i]*sizeof(double));
			assert(PI[i]!=NULL);
			PR[i]=malloc(count[i]*sizeof(double));
			assert(PR[i]!=NULL);
		}
		else {
			edges[i] = NULL;
			PS[i] = NULL;
			PI[i] = NULL;
			PR[i] = NULL;
		}
	}

	long *temp_count=calloc(maxId+1, sizeof(long));
	assert(temp_count!=NULL);

	for(i=0; i<linesNum; ++i) {
		int i1 =lines[i].i1;
		edges[i1][temp_count[i1]]=lines[i].i2;
		PS[i1][temp_count[i1]]=lines[i].d3;
		PI[i1][temp_count[i1]]=lines[i].d4;
		PR[i1][temp_count[i1]]=lines[i].d5;
		++temp_count[i1];
	}
	free(temp_count);
	for(i=0; i<maxId+1; ++i) {
		if (count[i] > 0) {
			quick_sort_int_index3(edges[i], 0, count[i]-1, PS[i], PI[i], PR[i]);
		}
	}

	net.maxId=maxId;
	net.minId=minId;
	net.edgesNum=linesNum;
	net.vtsNum=vtsNum;
	net.countMax=countMax;
	net.count=count;
	net.edges=edges;
	net.PS = PS;
	net.PI = PI;
	net.PR = PR;
	printf("build net:\n\tMax: %d, Min: %d, vtsNum: %d, edgesNum: %ld, countMax: %ld\n", maxId, minId, vtsNum, linesNum, countMax); fflush(stdout);
}

//status = 0, net.PS[i][eye].
//status = 1, net.PI[i][eye].
//status = 2, net.PR[i][eye].
double find_Net_PSPIPR(int infect_source, int eye, int status) {
	long i;
	if (status == 0) {
		for (i=0; i<net.count[infect_source]; ++i) {
			int node = net.edges[infect_source][i];
			if (eye < node) {
				continue;
			}
			else if (eye == node) {
				return net.PS[infect_source][i];
			}
			else {
				return 1;
			}
		}
		return 1;
	}
	else if (status == 1) {
		for (i=0; i<net.count[infect_source]; ++i) {
			int node = net.edges[infect_source][i];
			if (eye < node) {
				continue;
			}
			else if (eye == node) {
				return net.PI[infect_source][i];
			}
			else {
				return 0;
			}
		}
		return 0;
	}
	else {
		for (i=0; i<net.count[infect_source]; ++i) {
			int node = net.edges[infect_source][i];
			if (eye < node) {
				continue;
			}
			else if (eye == node) {
				return net.PR[infect_source][i];
			}
			else {
				return 0;
			}
		}
		return 0;
	}
}
