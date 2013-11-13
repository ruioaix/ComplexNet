#include "../../inc/compact/net.h"
#include "../../inc/utility/error.h"
#include "../../inc/utility/random.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

void free_Net(struct Net *net) {
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

struct Net *create_Net(const struct iiLineFile * const file) {
	int maxId=file->i1Max>file->i2Max?file->i1Max:file->i2Max;
	int minId=file->i1Min<file->i2Min?file->i1Min:file->i2Min;
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

	struct Net *net = malloc(sizeof(struct Net));
	assert(net != NULL);
	net->maxId=maxId;
	net->minId=minId;
	net->edgesNum=linesNum;
	net->vtsNum=vtsNum;
	net->countMax=countMax;
	net->count=count;
	net->edges=edges;
	printf("build net:\n\tMax: %d, Min: %d, vtsNum: %d, edgesNum: %ld, countMax: %ld\n", maxId, minId, vtsNum, linesNum, countMax); fflush(stdout);
	return net;
}

void *verifyNet(void *arg) {
	struct Net *net = arg;
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
		printf("verifyNet: perfect network.\n");
	}
	return (void *)0;
}

static void net_dmp_core(struct Net *net, int infect_source, int T, double infect_rate, double recover_rate, double **P1, double **P2, double **Theta, double **Phi, double *PS, double *PI, double *PR, double *theta_temp) {
	int maxId = net->maxId;
	int i;
	long j, k;
	int step = 1;

	char *sign = calloc(net->maxId + 1, sizeof(char));
	assert(sign != NULL);

	int *affectdata = malloc((net->maxId + 1)*sizeof(int));
	assert(affectdata != NULL);

	int affectdataNum = 0;

	while (step <= T) {
		//compute theta, phi, PSi->j
		for (i=0; i<maxId+1; ++i) {
			for (j=0; j<net->count[i]; ++j) {
				Theta[i][j] = Theta[i][j] - infect_rate*Phi[i][j];
			}
		}
		for (i=0; i<maxId+1; ++i) {
			for (j=0; j<net->count[i]; ++j) {
				long index = net_find_index(net, net->edges[i][j], i);
				if (index == -1) isError("index == -1");
				theta_temp[j] = Theta[net->edges[i][j]][index];
			}

			if (i != infect_source) {
				for (j=0; j<net->count[i]; ++j) {
					P2[i][j] = 1;
					for (k=0; k<net->count[i]; ++k) {
						if (j != k) {
							P2[i][j] *= theta_temp[k];
						}
					}
				}
			}
			else {
				for (j=0; j<net->count[i]; ++j) {
					P2[i][j] = 0;
				}
			}
		}
		for (i=0; i<maxId+1; ++i) {
			for (j=0; j<net->count[i]; ++j) {
				Phi[i][j] = (1-infect_rate)*(1-recover_rate)*Phi[i][j] + P1[i][j] - P2[i][j];
			}
		}

		
		//compute and store PS PI PR.
		for (i=0; i<maxId+1; ++i) {
			if (i!=infect_source) {
				PS[i] = 1;
				for (j=0; j<net->count[i]; ++j) {
					int jj = net->edges[i][j];
					long index = net_find_index(net, jj,i);
					if (index == -1) isError("PS index == -1");
					PS[i] *= Theta[jj][index];
				}
			}
			else {
				PS[i] = 0;
			}
			PR[i] = PR[i] + recover_rate*PI[i];
			PI[i] = 1 - PS[i] - PR[i];
		}
		
		double **temp; temp = P1; P1 = P2; P2 = temp;
		++step;
	}
}

static void net_dmp_init(struct Net *net, int infect_source, double **P1, double **P2, double **Theta, double **Phi, double *PS, double *PI, double *PR) {
	//init the P1 P2 Theta Phi
	int i;
	long j;
	for (i=0; i<net->maxId + 1; ++i) {
		//i is S
		if (i != infect_source) {
			for (j=0; j<net->count[i]; ++j) {
				P1[i][j] = 1;
				Phi[i][j] = 0;
				Theta[i][j] = 1;
			}
		}
		//i is I
		else {
			for (j=0; j<net->count[i]; ++j) {
				P1[i][j] = 0;
				Phi[i][j] = 1;
				Theta[i][j] = 1;
			}
		}
	}
	//init PS PI PR
	for (i=0; i<net->maxId+1; ++i) {
		PR[i] = 0;
		if (i!=infect_source) {
			PS[i] = 1;
			PI[i] = 0;
		}
		else {
			PS[i] = 0;
			PI[i] = 1;
		}
	}
}

void net_dmp(struct Net *net, int T, double infect_rate, double recover_rate) {
	double **P1 = malloc((net->maxId+1)*sizeof(void *));
	assert(P1 != NULL);
	double **P2 = malloc((net->maxId+1)*sizeof(void *));
	assert(P2 != NULL);
	double **Theta = malloc((net->maxId+1)*sizeof(void *));
	assert(Theta != NULL);
	double **Phi = malloc((net->maxId+1)*sizeof(void *));
	assert(Phi != NULL);

	double *PS = malloc((net->maxId+1)*sizeof(double));
	assert(PS != NULL);
	double *PI = malloc((net->maxId+1)*sizeof(double));
	assert(PI != NULL);
	double *PR = malloc((net->maxId+1)*sizeof(double));
	assert(PR != NULL);

	int i;
	for (i=0; i<net->maxId+1; ++i) {
		if (net->count[i] > 0) {
			P1[i] = malloc(net->count[i]*sizeof(double));
			assert(P1[i] != NULL);
			P2[i] = malloc(net->count[i]*sizeof(double));
			assert(P2[i] != NULL);
			Theta[i] = malloc(net->count[i]*sizeof(double));
			assert(Theta[i] != NULL);
			Phi[i] = malloc(net->count[i]*sizeof(double));
			assert(Phi[i] != NULL);
		}
		else {
			P1[i] = NULL;
			P2[i] = NULL;
			Theta[i] = NULL;
			Phi[i] = NULL;
		}
	}

	double *theta_temp = malloc((net->maxId+1)*sizeof(double));
	assert(theta_temp != NULL);

	char filename[100];
	sprintf(filename, "Results/PS_PI_PR_Time_%d.txt", T);
	FILE *fp = fopen(filename, "write");
	fileError(fp, "net_dmp");
	int j;
	for (i=0; i<net->maxId+1; ++i) {
		if (net->count[i] > 0) {
			net_dmp_init(net, i, P1, P2, Theta, Phi, PS, PI, PR);
			net_dmp_core(net, i, T, infect_rate, recover_rate, P1, P2, Theta, Phi, PS, PI, PR, theta_temp);
			for (j=0; j<net->maxId+1; ++j) {
				if (PS[j] != 1 || PI[j] != 0 || PR[j] != 0) {
					fprintf(fp, "%d, %d, %0.17f, %0.17f, %0.17f\n", i, j, PS[j], PI[j], PR[j]);	
				}
			}
			if (i%1000 == 0) {
				printf("%d\n", i);
				fflush(stdout);
			}
		}
	}
	fclose(fp);

	for (i=0; i<net->maxId+1; ++i) {
		if (net->count[i] > 0) {
			free(P1[i]);
			free(P2[i]);
			free(Theta[i]);
			free(Phi[i]);
		}
	}
	free(theta_temp);
	free(P1);
	free(P2);
	free(Theta);
	free(Phi);
	free(PS);
	free(PI);
	free(PR);
}

double *net_dmp_is(struct Net *net, int infect_source, int T, double infect_rate, double recover_rate) {
	double **P1 = malloc((net->maxId+1)*sizeof(void *));
	assert(P1 != NULL);
	double **P2 = malloc((net->maxId+1)*sizeof(void *));
	assert(P2 != NULL);
	double **Theta = malloc((net->maxId+1)*sizeof(void *));
	assert(Theta != NULL);
	double **Phi = malloc((net->maxId+1)*sizeof(void *));
	assert(Phi != NULL);

	double *PS = malloc(3*(net->maxId+1)*sizeof(double));
	assert(PS != NULL);
	double *PI = PS + net->maxId + 1;
	double *PR = PI + net->maxId + 1;

	int i;
	for (i=0; i<net->maxId+1; ++i) {
		if (net->count[i] > 0) {
			P1[i] = malloc(net->count[i]*sizeof(double));
			assert(P1[i] != NULL);
			P2[i] = malloc(net->count[i]*sizeof(double));
			assert(P2[i] != NULL);
			Theta[i] = malloc(net->count[i]*sizeof(double));
			assert(Theta[i] != NULL);
			Phi[i] = malloc(net->count[i]*sizeof(double));
			assert(Phi[i] != NULL);
		}
		else {
			P1[i] = NULL;
			P2[i] = NULL;
			Theta[i] = NULL;
			Phi[i] = NULL;
		}
	}

	double *theta_temp = malloc((net->maxId+1)*sizeof(double));
	assert(theta_temp != NULL);

	if (net->count[infect_source] > 0) {
		net_dmp_init(net, infect_source, P1, P2, Theta, Phi, PS, PI, PR);
		net_dmp_core(net, infect_source, T, infect_rate, recover_rate, P1, P2, Theta, Phi, PS, PI, PR, theta_temp);
	}

	for (i=0; i<net->maxId+1; ++i) {
		if (net->count[i] > 0) {
			free(P1[i]);
			free(P2[i]);
			free(Theta[i]);
			free(Phi[i]);
		}
	}
	free(theta_temp);
	free(P1);
	free(P2);
	free(Theta);
	free(Phi);

	return PS;
}

// find v2's index in net->edges[v1][index] = v2.
long net_find_index(struct Net *net, int v1, int v2) {
	if (v1 < net->minId || v1 > net->maxId) {
		return -1;
	}
	long i;
	for (i=0; i<net->count[v1]; ++i) {
		if (v2 == net->edges[v1][i]) {
			return i;
		}
	}
	return -1;
}

void print_Net(struct Net *net, char *filename) {
	FILE *fp = fopen(filename, "w");
	fileError(fp, "print_Net");
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
	printf("print_Net %s done. %ld lines generated.\n", filename, net->edgesNum);fflush(stdout);
}
