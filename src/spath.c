#include "spath.h"
#include "base.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "limits.h"

#define EPS 0.0000001

static void useRate_core_Net(double *sp, char *gs, int *blist, int *nlist, int bNum, int nNum, int **left, int **right, int *lNum, int *rNum, struct iiNet *net, struct iidNet *air, int *STEP_END) {
	int i,j;
	int STEP = 1;
	while (*lNum && STEP != *STEP_END) {
		*rNum = 0;
		++STEP;

		/********************************************************************************************************/
		for (i = 0; i < nNum; ++i) {
			int id = nlist[i];
			if (gs[id] != 3) {
				if (i != nNum - 1) {
					nlist[i] = nlist[--nNum];
					--i;
				}
				else {
					--nNum;
				}
			}
			else if(sp[id] < STEP + 1 - EPS && sp[id] >= STEP) {
				blist[bNum++] = id;
				gs[id] = 2;
				if (i != nNum - 1) {
					nlist[i] = nlist[--nNum];
					--i;
				}
				else {
					--nNum;
				}
			}
			else if (sp[id] >= STEP + 1 - EPS && sp[id] < STEP + 1 ) {
				gs[id] = 3;
				sp[id] = STEP + 1;
			}
			else if (sp[id] >= STEP + 1) {
			}
			else {
				printf("%d\t%f\t%d\n", gs[id], sp[id], STEP);
				isError("useRate_core_Net nlist");
			}
		}
		/********************************************************************************************************/

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<net->count[id]; ++j) {
				int neigh = net->edges[id][j];
				//if gs = 1, neigh is already defined.
				if (gs[neigh] != 1) {
					sp[neigh] = dmin(sp[id] + 1, sp[neigh]);
					if (sp[neigh] == STEP) {
						(*right)[(*rNum)++] = neigh;
						gs[neigh] = 1;
					}
					else if (sp[neigh] < STEP + 1 && sp[neigh] > STEP) {
						if (gs[neigh] != 2) {
							gs[neigh] = 2;
							blist[bNum++] = neigh;
						}
					}
					else {
						printf("err: %d\t%.17f\t%.17f\t%d\n", STEP, sp[id], sp[neigh], gs[neigh]);
						isError("useRate_core_Net, tt");
					}
				}
			}
			if(id < air->maxId + 1) {
				for (j=0; j<air->count[id]; ++j) {
					int neigh = air->edges[id][j];
					double airl = air->d[id][j];
					if (gs[neigh] != 1) {
						sp[neigh] = dmin(sp[id] + airl, sp[neigh]);
						if (sp[neigh] == STEP) {
							(*right)[(*rNum)++] = neigh;
							gs[neigh] = 1;
						}
						else if (sp[neigh] < STEP + 1 && sp[neigh] > STEP) {
							if (gs[neigh] != 2) {
								gs[neigh] = 2;
								blist[bNum++] = neigh;
							}
						}
						else if (sp[neigh] >= STEP + 1) {
							if (gs[neigh] == 0) {
								nlist[nNum++] = neigh;
								gs[neigh] = 3;
							}
							else if (gs[neigh] == 3) {
							}
							else {
								isError("useRate_core_Net x");
							}
						}
						else {
							printf("err: %d\t%d\t%d\t%f\n", STEP, id, neigh, sp[neigh]);
							isError("useRate_core_Net 2");
						}
					}
				}
			}
		}

		for (j = 0; j < bNum; ++j) {
			int id = blist[j];	
			if (gs[id] == 2) {
				(*right)[(*rNum)++] = id;
				gs[id] = 1;
			}
			else if (gs[id] == 1) {
			}
			else {
				printf("err: %d\t%d\t%f\n", gs[id], STEP, sp[id]);
				isError("useRate_core_Net 3");
			}
		}
		bNum = 0;

		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}

void get_kind01_SP(struct iiNet *net, struct iidNet *air, double *avesp) {
	double *sp = malloc((net->maxId + 1)*sizeof(double));
	assert(sp != NULL);
	int *left = malloc((net->maxId + 1)*sizeof(int));
	assert(left != NULL);
	int *right = malloc((net->maxId + 1)*sizeof(int));
	assert(right != NULL);
	int lNum, rNum;

	char *gs = malloc((net->maxId + 1)*sizeof(char));
	int *blist = malloc((net->maxId + 1)*sizeof(int));
	int *nlist = malloc((net->maxId + 1)*sizeof(int)); int bNum, nNum;

	*avesp = 0;
	int cc = 0;

	int i,j;
	int STEP_END = -1;
	//FILE *fp = fopen("1", "w"); fileError(fp, "get_avesp_Net");
	for (i=0; i<net->maxId + 1; ++i) {
		//printf("complete: %.4f%%\r", (double)i*100/(net->maxId + 1));fflush(stdout);
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = INT_MAX;
			gs[j] = 0;
		}
		sp[i] = -1;
		gs[i] = 1;
		bNum = nNum = lNum = 0;
		for (j = 0; j < net->count[i]; ++j) {
			int to = net->edges[i][j];
			left[lNum++] = to;
			sp[to] = 1.0;
			gs[to] = 1;
		}
		if (i < air->maxId + 1) {
			for (j = 0; j < air->count[i]; ++j) {
				int to = air->edges[i][j];
				double airl = air->d[i][j];
				if (gs[to] == 0) {
					if (airl < 2) {
						sp[to] = airl;
						gs[to] = 1;
						left[lNum++] = to;
					}
					else {
						sp[to] = airl;
						gs[to] = 3;
						nlist[nNum++] = to;
					}
				}
			}
		}
		useRate_core_Net(sp, gs, blist, nlist, bNum, nNum, &left, &right, &lNum, &rNum, net, air, &STEP_END);
		for (j = 0; j < net->maxId + 1; ++j) {
			//fprintf(fp, "sp: %d\t%d\t%f\n", i, j, sp[j]);
			if (sp[j] > 0) {
				*avesp += sp[j];
				++cc;
			}
		}
	}
	//fclose(fp);

	free(left);
	free(right);
	free(sp);
	free(gs);
	free(blist);
	free(nlist);
	double all = (double)(net->maxId + 1)*net->maxId;
	*avesp = (*avesp)/all;
	//printf("check: %d\t%f\n", cc, all);
}

double *get_kind02_SP(struct iiNet *net, struct iidNet *air, int i) {
	if (i < 0 || i > net->maxId) return NULL;

	double *sp = malloc((net->maxId + 1)*sizeof(double));
	assert(sp != NULL);
	int *left = malloc((net->maxId + 1)*sizeof(int));
	assert(left != NULL);
	int *right = malloc((net->maxId + 1)*sizeof(int));
	assert(right != NULL);
	int lNum, rNum;

	char *gs = malloc((net->maxId + 1)*sizeof(char));
	int *blist = malloc((net->maxId + 1)*sizeof(int));
	int *nlist = malloc((net->maxId + 1)*sizeof(int)); int bNum, nNum;

	int j;
	int STEP_END = -1;
	//FILE *fp = fopen("1", "w"); fileError(fp, "get_avesp_Net");

	//printf("complete: %.4f%%\r", (double)i*100/(net->maxId + 1));fflush(stdout);
	for (j=0; j<net->maxId + 1; ++j) {
		sp[j] = INT_MAX;
		gs[j] = 0;
	}
	sp[i] = -1;
	gs[i] = 1;
	bNum = nNum = lNum = 0;
	for (j = 0; j < net->count[i]; ++j) {
		int to = net->edges[i][j];
		left[lNum++] = to;
		sp[to] = 1.0;
		gs[to] = 1;
	}
	if (i < air->maxId + 1) {
		for (j = 0; j < air->count[i]; ++j) {
			int to = air->edges[i][j];
			double airl = air->d[i][j];
			if (gs[to] == 0) {
				if (airl < 2) {
					sp[to] = airl;
					gs[to] = 1;
					left[lNum++] = to;
				}
				else {
					sp[to] = airl;
					gs[to] = 3;
					nlist[nNum++] = to;
				}
			}
		}
	}
	useRate_core_Net(sp, gs, blist, nlist, bNum, nNum, &left, &right, &lNum, &rNum, net, air, &STEP_END);

	free(left);
	free(right);
	free(gs);
	free(blist);
	free(nlist);
	return sp;
}

static double ** dijkstra_setasp_SP(struct iidNet *net) {
	double **asp = malloc((net->maxId + 1) * sizeof(void *));
	int i, j;
	for (i = 0; i < net->maxId + 1; ++i) {
		asp[i] = malloc((net->maxId + 1) * sizeof(double));
	}
	for (i = 0; i < net->maxId + 1; ++i) {
		for (j = 0; j < net->maxId + 1; ++j) {
			asp[i][j] = INT_MAX;
		}
	}
	for (i = 0; i < net->maxId + 1; ++i) {
		for (j = 0; j < net->count[i]; ++j) {
			int id = net->edges[i][j];
			asp[i][id] = net->d[i][j];
		}
	}
	return asp;
}

double *dijkstra_1toall_SP(struct iidNet *net, int nid) {
	if (nid < 0 || nid > net->maxId) return NULL;
	double **asp = dijkstra_setasp_SP(net);
	double *sp = malloc((net->maxId + 1) * sizeof(double));
	assert(sp != NULL);
	char *flag = malloc(net->maxId + 1);
	assert(flag != NULL);
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		sp[i] = asp[nid][i];
		flag[i] = 0;
	}
	flag[nid] = 1;
	sp[nid] = -1;
	int alreadyflag = 1;
	while (alreadyflag != net->maxId + 1) {
		int be = -1;
		double min = INT_MAX;
		min *= 2;
		for (i = 0; i < net->maxId + 1; ++i) {
			if (!flag[i] && min > sp[i]) {
				min = sp[i];
				be = i;
			}
		}
		flag[be] = 1;
		++alreadyflag;
		for (i = 0; i < net->maxId + 1; ++i) {
			if (!flag[i] && sp[i] > sp[be] + asp[be][i]) {
				sp[i] = sp[be] + asp[be][i];
			}
		}
	}
	
	free(flag);
	for (i = 0; i < net->maxId + 1; ++i) {
		free(asp[i]);
	}
	free(asp);

	return sp;
}
