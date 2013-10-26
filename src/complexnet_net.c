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

	//init the P1 P2 Theta Phi
	for (i=0; i<maxId + 1; ++i) {
		//i is S
		if (i != x) {
			for (j=0; j<net.count[i]; ++j) {
				P1[i][j] = 1;
				Phi[i][j] = 0;
				Theta[i][j] = 1;
			}
		}
		//i is I
		else {
			for (j=0; j<net.count[i]; ++j) {
				P1[i][j] = 0;
				Phi[i][j] = 1;
				Theta[i][j] = 1;
			}
		}
	}

	double infect_rate = 0.5;
	double recover_rate = 0.5;
	int T = 10;
	double *PS = malloc((maxId+1)*sizeof(double));
	assert(PS != NULL);
	double *PI = malloc((maxId+1)*sizeof(double));
	assert(PI != NULL);
	double *PR = malloc((maxId+1)*sizeof(double));
	assert(PR != NULL);

	//init PS PI PR
	for (i=0; i<maxId+1; ++i) {
		PR[i] = 0;
		if (i!=x) {
			PS[i] = 1;
			PI[i] = 0;
		}
		else {
			PS[i] = 0;
			PI[i] = 1;
		}
	}

	long k;
	int step = 1;
	while (T > step) {
		//compute theta, phi, PSi->j
		for (i=0; i<maxId+1; ++i) {
			for (j=0; j<net.count[i]; ++j) {
				Theta[i][j] = Theta[i][j] - infect_rate*Phi[i][j];
				if (Theta[i][j] < 0) {
				//if (i == 25 && j == 4462) {
					printf("%d, %ld, theta\n", i,j);
				}
			}
		}
		for (i=0; i<maxId+1; ++i) {
			if (i != x) {
				for (j=0; j<net.count[i]; ++j) {
					P2[i][j] = 1;
					for (k=0; k<net.count[i]; ++k) {
						if (j != k) {
							int kk = net.edges[i][k];
							int index = net_find_index(kk, i);
							if (index == -1) isError("index == -1");

							P2[i][j] *= Theta[kk][index];
							if (P2[i][j] < 0) {
								printf("%d, %d, %f,P2\n", kk,i, P2[i][j]);
							}
						}
					}
				}
			}
			else {
				for (j=0; j<net.count[i]; ++j) {
					P2[i][j] = 0;
				}
			}
		}
		for (i=0; i<maxId+1; ++i) {
			for (j=0; j<net.count[i]; ++j) {
				Phi[i][j] = (1-infect_rate)*(1-recover_rate)*Phi[i][j] + P1[i][j] - P2[i][j];
							if (Phi[i][j] < 0) {
								printf("%d, %ld, Phi\n", i,j);
								printf("%f, %f, P1P2\n", P1[i][j], P2[i][j]);
							}
			}
		}

		
		//compute and store PS PI PR.
		char filename[100];
		sprintf(filename, "Results/PS_PI_PR_t_%d_status.txt", step);
		FILE *fp = fopen(filename, "write");
		fileError(fp, "net_dmp");
		for (i=0; i<maxId+1; ++i) {
			if (i!=x) {
				PS[i] = 1;
				for (j=0; j<net.count[i]; ++j) {
					int jj = net.edges[i][j];
					int index = net_find_index(jj,i);
					if (index == -1) isError("PS index == -1");
					PS[i] *= Theta[jj][index];
				}
			}
			else {
				PS[i] = 0;
			}
			PR[i] = PR[i] + recover_rate*PI[i];
			PI[i] = 1 - PS[i] - PR[i];
			fprintf(fp, "%d, %f, %f, %f\n", step, PS[i], PI[i], PR[i]);	
		}
		fclose(fp);
		
		double **temp; temp = P1; P1 = P2; P2 = temp;
		if (step == 2) {
			//exit(0);
		}
		++step;
	}

}

// find v2's index in net.edges[v1][index] = v2.
long net_find_index(int v1, int v2) {
	if (v1 < net.minId || v1 > net.maxId) {
		return -1;
	}
	long i;
	for (i=0; i<net.count[v1]; ++i) {
		if (v2 == net.edges[v1][i]) {
			return i;
		}
	}
	return -1;
}
