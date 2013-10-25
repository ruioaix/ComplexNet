#include "../inc/complexnet_net.h"
#include "../inc/complexnet_error.h"
#include "../inc/complexnet_threadpool.h"
#include "../inc/complexnet_random.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

static struct Net net;

struct Net *get_Net(void) {
	return &net;
}

void free_Net(void) {
	int i=0;
	for(i=0; i<net.maxId+1; ++i) {
		if (net.count[i]>0) {
			free(net.edges[i]);
		}
	}
	free(net.count);
	free(net.edges);
}

void create_Net(const struct iiLineFile * const file) {
	int maxId=file->iMax;
	int minId=file->iMin;
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
	int vtsNum=0;
	for(j=0; j<maxId+1; ++j) {
		if (count[j]>0) {
			++vtsNum;
		}
	}


	int **edges=malloc((maxId+1)*sizeof(void *));
	assert(edges!=NULL);
	long countMax=0;
	for(i=0; i<maxId+1; ++i) {
		if (countMax<count[i]) {
			countMax=count[i];
		}
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

	net.maxId=maxId;
	net.minId=minId;
	net.edgesNum=linesNum;
	net.vtsNum=vtsNum;
	net.countMax=countMax;
	net.count=count;
	net.edges=edges;
	printf("build net:\n\tMax: %d, Min: %d, vtsNum: %d, edgesNum: %ld, countMax: %ld\n", maxId, minId, vtsNum, linesNum, countMax); fflush(stdout);
}

void *verifyNet(void *arg) {
	long i;
	int j,k;
	int *place = malloc((net.maxId+1)*sizeof(int));
	FILE *fp = fopen("data/duplicatePairsinNet", "w");
	fileError(fp, "data/duplicatePairsinNet");
	FILE *fp2 = fopen("data/NoDuplicatePairsNetFile", "w");
	fileError(fp2, "data/NoDuplicatePairsNetFile");
	fprintf(fp, "the following pairs are duplicate in the net file\n");
	char sign=0;
	for (j=0; j<net.maxId+1; ++j) {
		if (net.count[j]>0) {
			for (k=0; k<net.maxId + 1; ++k) {
				place[k] = -1;
			}
			for (i=0; i<net.count[j]; ++i) {
				int origin = net.edges[j][i];
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
	}
	free(place);
	fclose(fp);
	fclose(fp2);
	if (sign == 1) {
		isError("the file has duplicate pairs, you can check data/duplicatePairsinNet.\nwe generate a net file named data/NoDuplicatePairsNetFile which doesn't contain any duplicate pairsr.\nyou should use this file instead the origin wrong one.\n");
	}
	else {
		printf("verifyNet: perfect network.\n");
	}
	return (void *)0;
}

void net_dmp(void) {
	double **P1 = malloc((net.maxId+1)*sizeof(void *));
	assert(P1 != NULL);
	double **P2 = malloc((net.maxId+1)*sizeof(void *));
	assert(P2 != NULL);
	double **Theta = malloc((net.maxId+1)*sizeof(void *));
	assert(Theta != NULL);
	double **Phi = malloc((net.maxId+1)*sizeof(void *));
	assert(Phi != NULL);

	int maxId = net.maxId;
	int i;
	for (i=0; i<maxId+1; ++i) {
		if (net.count[i] > 0) {
			P1[i] = malloc(net.count[i]*sizeof(double));
			assert(P1[i] != NULL);
			P2[i] = malloc(net.count[i]*sizeof(double));
			assert(P2[i] != NULL);
			Theta[i] = malloc(net.count[i]*sizeof(double));
			assert(Theta[i] != NULL);
			Phi[i] = malloc(net.count[i]*sizeof(double));
			assert(Phi[i] != NULL);
		}
		else {
			P1[i] = NULL;
			P2[i] = NULL;
			Theta[i] = NULL;
			Phi[i] = NULL;
		}
	}


	int x=12;
	long j;
	for (i=0; i<maxId + 1; ++i) {
		for (j=0; j<net.count[i]; ++j) {
			if (i != x) {
				P1[i][j] = 1;
				Phi[i][j] = 1;
			}
			else {
				P1[i][j] = 0;
				Phi[i][j] = 0;
			}
			Theta[i][j] = 1;
		}
	}

	double infect_rate = 0.5;
	double recover_rate = 0.5;
	int step = 10;

	long k;
	while (step-- == 0) {
		for (i=0; i<maxId+1; ++i) {
			for (j=0; j<net.count[i]; ++j) {
				Theta[i][j] = Theta[i][j] - infect_rate*Phi[i][j];
			}
		}
		for (i=0; i<maxId+1; ++i) {
			if (i != x) {
				for (j=0; j<net.count[i]; ++j) {
					P2[i][j] = 1;
					for (k=0; k<net.count[i]; ++k) {
						if (j != k) {
							P2[i][j] *= Theta[i][j];
						}
					}
				}
			}
			else {
				P2[i][j] = 0;
			}
		}
		for (i=0; i<maxId+1; ++i) {
			for (j=0; j<net.count[i]; ++j) {
				Phi[i][j] = (1-infect_rate)*(1-recover_rate)*Phi[i][j] + P1[i][j] - P2[i][j];
			}
		}

	}

}
