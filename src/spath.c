#include "spath.h"
#include "base.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "limits.h"

#define EPS 0.0000001

//standard dijkstra algorithm
static double ** dijkstra_setasp_iidNet(struct iidNet *net) {
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
double *dijkstra_1A_iidNet(struct iidNet *net, int nid) {
	if (nid < 0 || nid > net->maxId) return NULL;
	double **asp = dijkstra_setasp_iidNet(net);
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

//this spath01 is for unweighted and undirected net.
static void spath01_core_iiNet(int *sp, int **left, int **right, int *lNum, int *rNum, struct iiNet *net, int *STEP_END) {
	int i,j;
	int STEP = 0;
	while (*lNum && STEP != *STEP_END) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<net->count[id]; ++j) {
				int neigh = net->edges[id][j];
				if (!sp[neigh]) {
					sp[neigh] = STEP;
					(*right)[(*rNum)++] = neigh;
				}
			}
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}
int *spath01_1A_iiNet(struct iiNet *net, int originId) {
	if (originId<net->minId || originId>net->maxId) {
		return NULL;
	}
	int *sp = calloc(net->maxId + 1, sizeof(int));
	int *left = malloc((net->maxId + 1)*sizeof(int));
	int *right = malloc((net->maxId + 1)*sizeof(int));
	int lNum, rNum;
	lNum = 1;
	left[0] = originId;
	sp[originId] = -1;
	int STEP_END = -1;
	spath01_core_iiNet(sp, &left, &right, &lNum, &rNum, net, &STEP_END);
	free(left);
	free(right);
	return sp;	
}
int *spath01_step_1A_iiNet(struct iiNet *net, int originId, int step, int *Num) {
	if (originId<net->minId || originId>net->maxId) {
		return NULL;
	}
	int *sp = calloc(net->maxId + 1, sizeof(int));
	int *left = malloc((net->maxId + 1)*sizeof(int));
	int *right = malloc((net->maxId + 1)*sizeof(int));
	int lNum, rNum;
	lNum = 1;
	left[0] = originId;
	sp[originId] = -1;
	int STEP_END = step;
	spath01_core_iiNet(sp, &left, &right, &lNum, &rNum, net, &STEP_END);
	free(sp);
	free(right);
	*Num = lNum;
	return left;	
}
void avesp_spath01_iiNet(struct iiNet *net, double *avesp) {
	int *sp = malloc((net->maxId + 1)*sizeof(int));
	int *left = malloc((net->maxId + 1)*sizeof(int));
	int *right = malloc((net->maxId + 1)*sizeof(int));
	int lNum, rNum;

	int i,j;
	int STEP_END = -1;
	*avesp = 0;
	for (i=0; i<net->maxId + 1; ++i) {
		//printf("complete: %.4f%%\r", (double)i*100/(net->maxId + 1));fflush(stdout);
		lNum = 1;
		left[0] = i;
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = 0;
		}
		sp[i] = -1;
		spath01_core_iiNet(sp, &left, &right, &lNum, &rNum, net, &STEP_END);
		for (j=i+1; j<net->maxId + 1; ++j) {
			*avesp += sp[j];
		}
	}

	free(left);
	free(right);
	free(sp);
	double EE = (double)(net->maxId + 1)*(net->maxId)/2.0;
	*avesp /= EE;
}

//this spath02 is FW algorithm for unweighted and undirected net.
int **spath02_AA_iiNet(struct iiNet *net) {
	//FILE *fp = fopen("fw", "w");
	int **apsp = malloc((net->maxId + 1)*sizeof(void *));
	int i,j,k;
	for (i=0; i<net->maxId + 1; ++i) {
		apsp[i] = calloc((net->maxId + 1), sizeof(int));
	}
	for (i=0; i<net->maxId + 1; ++i) {
		for (j=0; j<net->count[i]; ++j) {
			int id = net->edges[i][j];
			apsp[i][id] = 1;
		}
	}
	for (i=0; i<net->maxId + 1; ++i) {
		for (j=0; j<net->maxId + 1; ++j) {
			if (!apsp[i][j]) {
				apsp[i][j] = 2*(net->maxId + 1);
			}
			else if (i<j){
				//fprintf(fp, "%d\t%d\t%d\n", i, j, apsp[i][j]);
			}
			if (i==j) {
				apsp[i][j] = 0;
			}
			//fprintf(fp, "%d\t%d\t%d\n", i, j, apsp[i][j]);
		}
	}
	for (k=0; k<net->maxId + 1; ++k) {
		printf("%.2f%%\r", k*100.0/net->maxId);fflush(stdout);
		for (i=0; i<net->maxId + 1; ++i) {
			for (j=0; j<net->maxId + 1; ++j) {
				int havek = apsp[i][k] + apsp[k][j];
				apsp[i][j] = havek < apsp[i][j] ? havek : apsp[i][j];
			}
		}
		//printf("%d\t%d\t%d\n", apsp[7][0], apsp[8][8], apsp[7][8]);
	}
	printf("\n");fflush(stdout);
	return apsp;
}

//this spath03 is for unweighted_undirected base and weighted_undirected air network.
//the weight of each link in air network is >= 1.0.
static void spath03_core_Net(double *sp, char *gs, int *blist, int *nlist, int bNum, int nNum, int **left, int **right, int *lNum, int *rNum, struct iiNet *net, struct iidNet *air, int *STEP_END) {
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
				isError("spath03_core_Net nlist");
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
						isError("spath03_core_Net, tt");
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
								isError("spath03_core_Net x");
							}
						}
						else {
							printf("err: %d\t%d\t%d\t%f\n", STEP, id, neigh, sp[neigh]);
							isError("spath03_core_Net 2");
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
				isError("spath03_core_Net 3");
			}
		}
		bNum = 0;

		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}
void avesp_spath03_Net(struct iiNet *net, struct iidNet *air, double *avesp) {
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
		spath03_core_Net(sp, gs, blist, nlist, bNum, nNum, &left, &right, &lNum, &rNum, net, air, &STEP_END);
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

//this spath04 is for unweighted and undirected net.
//to find how many shortest pathes use only base or only air or both.
static void spath04_core_iiNet(int *sp, char *use, int **left, int **right, int *lNum, int *rNum, struct iiNet *net, struct iiNet *air, int *STEP_END) {
	int i,j;
	int STEP = 1;
	while (*lNum && STEP != *STEP_END) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			//printf("id:%d\n", id);
			for (j=0; j<net->count[id]; ++j) {
				int neigh = net->edges[id][j];
				if (!sp[neigh]) {
					//pn q xiang yu net
					if (0 == use[id]) {
						if (-1 == use[neigh]) {
							use[neigh] = 3;
						}
						else if (4 == use[neigh]) {
							use[neigh] = 2;
							sp[neigh] = STEP;
							(*right)[(*rNum)++] = neigh;
						}
					}
					else if (1 == use[id]) {
						if (-1 == use[neigh]) {
							use[neigh] = 4;
						}
						else if (3 == use[neigh]) {
							use[neigh] = 2;
							sp[neigh] = STEP;
							(*right)[(*rNum)++] = neigh;
						}
					}
					else if (2 == use[id]) {
						use[neigh] = 2;
						sp[neigh] = STEP;
						(*right)[(*rNum)++] = neigh;
					}
				}
			}
			if(id < air->maxId + 1) {
				for (j=0; j<air->count[id]; ++j) {
					int neigh = air->edges[id][j];
					if (!sp[neigh]) {
						if (-1 == use[neigh]) {
							use[neigh] = 4;
						}
						else if (3 == use[neigh]) {
							use[neigh] = 2;
							sp[neigh] = STEP;
							(*right)[(*rNum)++] = neigh;
						}
					}
				}
			}
		}
		//static int kk = 0;
		for (j = 0; j < net->maxId + 1; ++j) {
			if (3 == use[j] || 4 == use[j]) {
				use[j] -= 3;
				sp[j] = STEP;
				(*right)[(*rNum)++] = j;
			}
			//printf("%d\t%d\n", j, use[j]);fflush(stdout);
		}
		//printf("******************************************\n");
		//if (kk++ == 4) exit(0);
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}
void useRate_spath04_iiNet(struct iiNet *net, struct iiNet *air, double *useRate, double *sameRate, double *cleanRate, double *avesp) {
	int *sp = malloc((net->maxId + 1)*sizeof(int));
	assert(sp != NULL);
	int *left = malloc((net->maxId + 1)*sizeof(int));
	assert(left != NULL);
	int *right = malloc((net->maxId + 1)*sizeof(int));
	assert(right != NULL);
	char *use = malloc((net->maxId + 1) * sizeof(char));
	assert(use != NULL);
	int lNum, rNum;

	int i,j;
	int STEP_END = -1;
	*useRate = 0;
	*sameRate = 0;
	*cleanRate = 0;
	*avesp = 0;
	for (i=0; i<net->maxId + 1; ++i) {
		//printf("complete: %.4f%%\r", (double)i*100/(net->maxId + 1));fflush(stdout);
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = 0;
			use[j] = -1;
		}
		sp[i] = -1;
		lNum = 0;
		for (j = 0; j < net->count[i]; ++j) {
			int to = net->edges[i][j];
			left[lNum++] = to;
			sp[to] = 1;
			use[to] = 0;
		}
		if (i < air->maxId + 1) {
			for (j = 0; j < air->count[i]; ++j) {
				int to = air->edges[i][j];
				left[lNum++] = to;
				sp[to] = 1;
				use[to] = 1;
			}
		}
		spath04_core_iiNet(sp, use, &left, &right, &lNum, &rNum, net, air, &STEP_END);
		sp[i] = 0;
		for (j = 0; j < net->maxId + 1; ++j) {
			if (1 == use[j]) {
				++(*useRate);
			}
			else if (2 == use[j]) {
				++(*sameRate);
			}
			else if (0 == use[j]) {
				++(*cleanRate);
			}
			*avesp += sp[j];
		}
	}

	free(left);
	free(right);
	free(sp);
	free(use);
	double all = (double)(net->maxId + 1)*net->maxId;
	*useRate /= all;
	*sameRate /= all;
	*cleanRate /= all;
	*avesp /= all;
}

//this spath05 is for unweighted and undirected net.
//to find coupling of two net: base and air.
static void spath05_core_iiNet(int *sp, char *stage, int **left, int **right, int *lNum, int *rNum, struct iiNet *net, struct iiNet *air, int *STEP_END, double *spa, double *spb, double *spab) {
	int i,j;
	int STEP = 1;
	memset(stage, 0 ,sizeof(char)*(net->maxId + 1));
	while (*lNum && STEP != *STEP_END) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			//printf("id:%d\n", id);
			for (j=0; j<net->count[id]; ++j) {
				int neigh = net->edges[id][j];
				if (!sp[neigh]) {
					spab[neigh] += spab[id];
					spab[neigh] += spa[id];
					spb[neigh] += spb[id];
					if (stage[neigh] == 0) {
						stage[neigh] = 1;
						(*right)[(*rNum)++] = neigh;
					}
				}
			}
			if(id < air->maxId + 1) {
				for (j=0; j<air->count[id]; ++j) {
					int neigh = air->edges[id][j];
					if (!sp[neigh]) {
						spab[neigh] += spab[id];
						spab[neigh] += spb[id];
						spa[neigh] += spa[id];
						if (stage[neigh] == 0) {
							stage[neigh] = 1;
							(*right)[(*rNum)++] = neigh;
						}
					}
				}
			}
		}
		for (j = 0; j < *rNum; ++j) {
			sp[(*right)[j]] = STEP;
			stage[(*right)[j]] = 0;
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}
void coupling_spath05_iiNet(struct iiNet *net, struct iiNet *air, double *coupling, double *avesp) {
	int *sp = smalloc((net->maxId + 1)*sizeof(int));
	int *left = smalloc((net->maxId + 1)*sizeof(int));
	int *right = smalloc((net->maxId + 1)*sizeof(int));
	double *spa = smalloc((net->maxId + 1) * sizeof(double));
	double *spb = smalloc((net->maxId + 1) * sizeof(double));
	double *spab = smalloc((net->maxId + 1) * sizeof(double));
	char *stage = smalloc((net->maxId + 1) * sizeof(char));
	int lNum, rNum;

	int i,j;
	int STEP_END = -1;
	*avesp = 0;
	for (i=0; i<net->maxId + 1; ++i) {
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = 0;
			spa[j] = 0;
			spb[j] = 0;
			spab[j] = 0;
		}
		sp[i] = -1;
		lNum = 0;
		for (j = 0; j < net->count[i]; ++j) {
			int to = net->edges[i][j];
			left[lNum++] = to;
			sp[to] = 1;
			++spb[to];
		}
		if (i < air->maxId + 1) {
			for (j = 0; j < air->count[i]; ++j) {
				int to = air->edges[i][j];
				left[lNum++] = to;
				sp[to] = 1;
				++spa[to];
			}
		}
		spath05_core_iiNet(sp, stage, &left, &right, &lNum, &rNum, net, air, &STEP_END, spa, spb, spab);
		for (j = i+1; j < net->maxId + 1; ++j) {
			*avesp += sp[j];
			*coupling += spab[j]/(spa[j] + spb[j] + spab[j]);
		}
	}

	free(left); free(right);
	free(spa); free(spb); free(spab);
	free(sp);
	free(stage);
	double ij = (double)(net->maxId + 1)*net->maxId/2;
	*avesp /= ij;
	*coupling /= ij;
}

//this spath06 is for unweighted_undirected base and weighted_undirected air network.
//to find gini of two net: base and air.
static void spath06_core01_Net(int *sp, char *stage,  int **left, int **right, int *lNum, int *rNum, struct iiNet *net, struct iidNet *XE, int *STEP_END, double *spall) {
	int i,j;
	int STEP = 1;
	memset(stage, 0 ,sizeof(char)*(net->maxId + 1));
	while (*lNum && STEP != *STEP_END) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<XE->count[id]; ++j) {
				int neigh = XE->edges[id][j];
				if (!sp[neigh]) {
					spall[neigh] += spall[id];
					if (stage[neigh] == 0) {
						stage[neigh] = 1;
						(*right)[(*rNum)++] = neigh;
					}
				}
			}
		}

		for (j = 0; j < *rNum; ++j) {
			sp[(*right)[j]] = STEP;
			stage[(*right)[j]] = 0;
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}
static void set_d_XE(struct iidNet *net, int from, int to, double d, int des, int sou) {
	if (from > net->maxId || to > net->maxId || from < 0 || to <0) return;
	//int sm = imin(from, to);
	//int bg = imax(from, to);
	int sm = from;
	int bg = to;
	int i;
	for (i = 0; i < net->count[sm]; ++i) {
		if (bg == net->edges[sm][i]) {
			net->d[sm][i] += d;
			//printf("%d\t%d\t%f\t%d\t%d\n", from, to, d, des, sou);
			break;
		}
	}
}
static void spath06_core02_Net(int source, int *sp, char *stage, int **left, int **right, int *lNum, int *rNum, struct iiNet *net, struct iidNet *XE, double *spall) {
	int i;
	int j;
	int k;
	*rNum = 0;
	memset(stage, 0 ,sizeof(char)*(XE->maxId + 1));
	for (i = 0; i < XE->maxId + 1; ++i) {
		int step = sp[i];
		double aij = spall[i];
		if (step == 1) {
			assert(aij == 1);
			set_d_XE(XE, i, source, 1, i, source);
		}
		else if (step > 1) {
			*lNum = 0;
			(*left)[(*lNum)++] = i;
			while (step != 1) {
				step--;
				*rNum = 0;

				for (k=0; k<*lNum; ++k) {
					int id = (*left)[k];
					//printf("id:%d\n", id);
					for (j=0; j<XE->count[id]; ++j) {
						int neigh = XE->edges[id][j];
					//for (j=0; j<net->count[id]; ++j) {
					//	int neigh = net->edges[id][j];
						if (sp[neigh] == step) {
							set_d_XE(XE, id, neigh, spall[neigh]/aij, i, source);
							if (stage[neigh] == 0) {
								stage[neigh] = 1;
								(*right)[(*rNum)++] = neigh;
							}
						}
					}
				}

				for (k = 0; k < *rNum; ++k) {
					int id = (*right)[k];
					stage[id] = 0;	
				}

				int *tmp = *left;
				*left = *right;
				*right = tmp;
				*lNum = *rNum;
			}
			for (k=0; k<*lNum; ++k) {
				int id = (*left)[k];
				set_d_XE(XE, id, source, 1/aij, i, source);
			}
		}
	}
}

double spath06_core03_Net(struct iidNet *net, struct iiNet *base) {
	int i,j;
	int m,n;
	double diff = 0.0;
	double total = 0.0;
	for (i = 0; i < net->maxId + 1; ++i) {
		for (j = 0; j < net->count[i]; ++j) {
			if (i<net->edges[i][j]) {
				double x1 = net->d[i][j];	
				total += x1;
				for (m = 0; m < net->maxId + 1; ++m) {
					for (n = 0; n < net->count[m]; ++n) {
						if (m < net->edges[m][n]) {
							double x2 = net->d[m][n];
							diff += fabs(x1 - x2);
						}
					}
				}
			}
		}
	}
	double E = (double)net->edgesNum;
	double G = diff/(2*E*total);
	return G;
}
void gini_spath06_Net(struct iiNet *net, struct iidNet *XE, double *avesp, double *gini) {
	int *sp = malloc((net->maxId + 1)*sizeof(int));
	assert(sp != NULL);
	int *left = malloc((net->maxId + 1)*sizeof(int));
	assert(left != NULL);
	int *right = malloc((net->maxId + 1)*sizeof(int));
	assert(right != NULL);
	double *spall = malloc((net->maxId + 1) * sizeof(double));
	assert(spall != NULL);
	char *stage = malloc((net->maxId + 1) * sizeof(char));
	assert(stage != NULL);
	int lNum, rNum;

	int i,j;
	int STEP_END = -1;
	double allsp = 0;
	for (i=0; i<net->maxId + 1; ++i) {
		//printf("complete: %.4f%%\r", (double)i*100/(net->maxId + 1));fflush(stdout);
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = 0;
			spall[j] = 0;
		}
		sp[i] = -1;
		lNum = 0;
		for (j = 0; j < XE->count[i]; ++j) {
			int to = XE->edges[i][j];
			left[lNum++] = to;
			sp[to] = 1;
			++spall[to];
		}
		spath06_core01_Net(sp, stage, &left, &right, &lNum, &rNum, net, XE, &STEP_END, spall);
		spath06_core02_Net(i, sp, stage, &left, &right, &lNum, &rNum, net, XE, spall);
		for (j = i+1; j < net->maxId + 1; ++j) {
			allsp += sp[j];
		}
	}

	free(left);
	free(right);
	free(sp);
	free(spall);
	free(stage);
	*avesp = allsp*2.0/((double)(net->maxId + 1)*net->maxId);
	*gini = spath06_core03_Net(XE, net);

}
