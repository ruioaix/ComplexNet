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
	int i, j;
	for(i=0; i<net.maxId+1; ++i) {
		for (j=0; j<net.maxId+1; ++j) {
			free(net.psir[i][j]);
		}
		free(net.psir[i]);
	}
	free(net.psir);
}

void create_Net_PSPIPR(const struct iid3LineFile * const file) {
	int maxId=file->iMax;
	int minId=file->iMin;
	long linesNum=file->linesNum;
	struct iid3Line *lines=file->lines;

	double ***psir=malloc((maxId+1)*sizeof(void *));
	assert(psir!=NULL);

	int i;
	for(i=0; i<maxId+1; ++i) {
		psir[i]=malloc((maxId+1)*sizeof(void *));
		assert(psir[i]!=NULL);
	}

	int x=0, y=0;
	for(i=0; i<linesNum; ++i) {
		int i1 =lines[i].i1;
		int i2 =lines[i].i2;
		while (x < i1) {
			for (; y<maxId+1; ++y) {
				psir[x][y] = NULL;	
			}
			++x;
			y=0;
		}
		while (y < i2) {
			for (; y<i2; ++y) {
				psir[x][y] = NULL;
			}
		}
		psir[x][y] = malloc(3*sizeof(double));
		assert(psir[x][y] != NULL);
		psir[x][y][0] = lines[i].d3;
		psir[x][y][1] = lines[i].d4;
		psir[x][y][2] = lines[i].d5;
		++y;
	}

	net.maxId=maxId;
	net.minId=minId;
	net.psir=psir;
	printf("Read PSPIPR Successfully\n"); fflush(stdout);
}

//status = 0, net.PS[i][eye].
//status = 1, net.PI[i][eye].
//status = 2, net.PR[i][eye].
//double find_Net_PSPIPR(int infect_source, int eye, int status) {
//	long i;
//	if (status == 0) {
//		for (i=0; i<net.count[infect_source]; ++i) {
//			int node = net.edges[infect_source][i];
//			if (eye > node) {
//				continue;
//			}
//			else if (eye == node) {
//				return net.PS[infect_source][i];
//			}
//			else {
//				return 1;
//			}
//		}
//		return 1;
//	}
//	else if (status == 1) {
//		for (i=0; i<net.count[infect_source]; ++i) {
//			int node = net.edges[infect_source][i];
//			if (eye > node) {
//				continue;
//			}
//			else if (eye == node) {
//				return net.PI[infect_source][i];
//			}
//			else {
//				return 0;
//			}
//		}
//		return 0;
//	}
//	else {
//		for (i=0; i<net.count[infect_source]; ++i) {
//			int node = net.edges[infect_source][i];
//			if (eye > node) {
//				continue;
//			}
//			else if (eye == node) {
//				return net.PR[infect_source][i];
//			}
//			else {
//				return 0;
//			}
//		}
//		return 0;
//	}
//}
