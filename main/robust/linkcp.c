//./robust-base 2 12345 1 20 1
#include "base.h"
#include "linefile.h"
#include "iinet.h"
#include "dataset.h"
#include "sort.h"
#include "mtprand.h"
#include "i3net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "robust.h"

struct CoupLink {
	struct LineFile *lid_i12;
	struct i3Net *i12_lid;
	int *lid_gid;
	int gidMax;
	int *gidCount;
	int **gid_lids;
};

int *robust_get_fg_2(struct LineFile *lf, double q, int **gidCounts) {
	if (q<0 || q>1) isError("robust_get_fg_2");
	
	long cplkNum = (long)(q*lf->linesNum);

	//printf("%d\n", cplkNum);

	int * fg = malloc(lf->linesNum * sizeof(int));
	int *gidCount = calloc(cplkNum, sizeof(int));
	int gid = 0;
	long i;
	for (i = 0; i < lf->linesNum; ++i) {
		fg[i] = -1;
	}
	long cpNum = 0;
	while (cpNum < cplkNum) {
		int id1 = get_i31_MTPR()%(lf->linesNum);
		int id2 = get_i31_MTPR()%(lf->linesNum);
		if (id1 == id2) continue;
		if (fg[id1] == -1 && fg[id2] == -1) {
			fg[id1] = fg[id2] = gid;
			gidCount[gid] += 2;
			cpNum += 2;
			++gid;
		}
		else if (fg[id1] == -1 && fg[id2] != -1) {
			fg[id1] = fg[id2];
			++gidCount[fg[id2]];
			++cpNum;
		}
		else if (fg[id1] != -1 && fg[id2] == -1) {
			fg[id2] = fg[id1];
			++gidCount[fg[id1]];
			++cpNum;
		}
		else if (fg[id1] == fg[id2]) {
			continue;
		}
		else {
			int sgid, bgid;
			if (gidCount[fg[id1]] > gidCount[fg[id2]]) {
				sgid = fg[id2];
				bgid = fg[id1];
			}
			else {
				sgid = fg[id1];
				bgid = fg[id2];
			}
			for (i = 0; i < lf->linesNum; ++i) {
				if (fg[i] == sgid) {
					fg[i] = bgid;
					--gidCount[sgid];
					++gidCount[bgid];
					if (gidCount[sgid] == 0) {
						break;
					}
				}
			}
		}
	}
		
	//for (i = 0; i < lf->linesNum; ++i) {
	//	assert(fg[i] == -1);
	//}
	//if (cplkNum == 0) {
	//	printf("%x\n", gidCount);
	//	assert(gidCount == NULL);
	//}
	*gidCounts = gidCount;
	return fg;
}

int *robust_get_fg(struct LineFile *lf, double q) {
	int * fg = malloc(lf->linesNum * sizeof(int));
	long *clean = malloc(lf->linesNum * sizeof(long));
	long cleanNum = lf->linesNum;
	int gid = 0;
	long i;
	for (i = 0; i < lf->linesNum; ++i) {
		fg[i] = -1;
		clean[i] = i;
	}
	for (i = 0; i < lf->linesNum; ++i) {
		if (cleanNum == 0) break;
		if(fg[i] == -1) {
			double psb = get_d_MTPR();
			if (psb < q) {
				int eid = get_i31_MTPR()%(lf->linesNum);
				if (fg[eid] == -1) {
					fg[eid] = fg[i] = gid++;
					clean[eid] = clean[--cleanNum];
					clean[i] = clean[--cleanNum];
				}
				else {
					fg[i] = fg[eid];
					clean[i] = clean[--cleanNum];
				}
			}
		}
		else {
			double psb = get_d_MTPR();
			if (psb < q) {
				int eid = clean[get_i31_MTPR()%cleanNum];
				fg[eid] = fg[i];
				clean[eid] = clean[--cleanNum];
			}
		}
	}
	free(clean);
	return fg;
}

int *robust_get_fg_1(struct LineFile *lf, double q, int coupNum) {
	int * fg = malloc(lf->linesNum * sizeof(int));
	long *clean = malloc(lf->linesNum * sizeof(long));
	long cleanNum = lf->linesNum;
	int gid = 0;
	long i;
	for (i = 0; i < lf->linesNum; ++i) {
		fg[i] = -1;
		clean[i] = i;
	}
	int j;
	for (i = 0; i < lf->linesNum; ++i) {
		if (cleanNum == 0) break;
		if (fg[i] != -1) continue;
		double psb = get_d_MTPR();
		if (psb < q) {
			for (j = 0; j < coupNum; ++j) {
				int index = get_i31_MTPR()%cleanNum;
				int eid = clean[index];
				fg[eid] = gid;
				clean[index] = clean[--cleanNum];
				//printf("gid: %d, eid: %d, fg[eid]: %d\n", gid, eid, fg[eid]);
				if (cleanNum == 0) {
					break;
				}
			}
			gid++;
		}
		//printf("\n");
	}
	free(clean);
	return fg;
}

void robust_get_linkcp(struct LineFile *lf, int *fg, int *maxgs, int *lcpCount, int ***lcps) {
	long rlNum = 0;
	int i;
	int maxg = -1;
	for (i = 0; i < lf->linesNum; ++i) {
		if (fg[i] == -1) continue;
		maxg = imax(maxg, fg[i]);
		lf->i1[rlNum] = lf->i1[i];
		lf->i2[rlNum] = lf->i2[i];
		fg[rlNum] = fg[i];
		++rlNum;
	}
	lf->linesNum = rlNum;
	//for (i = 0; i < rlNum; ++i) {
	//	//printf("i,fg[i]: %d\t%d\n", i, fg[i]);
	//}
	int **lcp = malloc((maxg + 1)*sizeof(int *));
	for (i = 0; i < maxg + 1; ++i) {
		lcp[i] = malloc(lcpCount[i]*sizeof(int));
	}
	int *tmpCount = calloc(maxg + 1, sizeof(int));
	for (i = 0; i < rlNum; ++i) {
		lcp[fg[i]][(tmpCount[fg[i]])++] = i;
	}
	free(tmpCount);
	*lcps = lcp;	
	*maxgs = maxg;
}

int find_lid_from_i12(int id, int neigh, struct i3Net *lcpnet) {
	//printf("%d\t%d\n", id, neigh);fflush(stdout);
	if (id > lcpnet->maxId) return -1;
	int i;
	for (i = 0; i < lcpnet->count[id]; ++i) {
		if (lcpnet->edges[id][i] == neigh) {
			return lcpnet->i3[id][i];
		}
	}
	//printf("end: %d\t%d\n", id, neigh);fflush(stdout);
	return -1;
}

static int get_cplk_in_maxRobust(int nid, struct iiNet *net, int *lstate, struct i3Net *i12_lid) {
	char *fg = calloc(net->maxId + 1, sizeof(char));
	int *left = malloc((net->maxId + 1) * sizeof(int));
	int *right = malloc((net->maxId + 1) * sizeof(int));

	int lk = 0;

	int lN = 0, rN = 0;
	left[lN++] = nid;
	fg[nid] = 1;
	int i;
	long j;
	while(lN) {
		rN = 0;
		for (i = 0; i < lN; ++i) {
			int id = left[i];
			for (j = 0; j < net->count[id]; ++j) {
				int neigh = net->edges[id][j];
				int lid = find_lid_from_i12(id, neigh, i12_lid);
				if (lid != -1) {
					assert(lstate[lid] != 2);
					if (lstate[lid] == 0) {
						lstate[lid] = 1;
						lk++;
					}
				}
				if (fg[neigh] == 0) {
					fg[neigh] = 1;
					right[rN++] = neigh;
				}
			}
		}
		int *tmp = left;
		left = right;
		right = tmp;
		lN = rN;
	}

	free(fg);
	free(left);
	free(right);
	return lk;
}

int delete_linkcp_iiNet(struct iiNet *net, struct CoupLink *cplk, int *lstate) {

	struct LineFile *lf = cplk->lid_i12;
	int *lid_gid = cplk->lid_gid;
	int **gid_lids = cplk->gid_lids;
	int *gidCount = cplk->gidCount;
	
	int t=0;
	int i, j;
	for (i = 0; i < lf->linesNum; ++i) {
		if (lstate[i] == 1) {
			int ssi = 0;
			for (j = 0; j < gidCount[lid_gid[i]]; ++j) {
				int id = gid_lids[lid_gid[i]][j];
				if (lstate[id] == 0 ) {
					assert(net->count[lf->i1[id]] != 0);
				    assert(net->count[lf->i2[id]] != 0);
					ssi = 1;
					break;
				}
			}
			if (ssi == 1) {
				for (j = 0; j < gidCount[lid_gid[i]]; ++j) {
					int id = gid_lids[lid_gid[i]][j];
					if (lstate[id] == 2) {
						isError("delete_linkcp_iiNet");
					}
					lstate[id] = 2;
					t += delete_link_iiNet(net, lf->i1[id], lf->i2[id]);
					
				}
			}
			else {
				for (j = 0; j < gidCount[lid_gid[i]]; ++j) {
					int id = gid_lids[lid_gid[i]][j];
					lstate[id] = 0;
				}
			}
		}
	}
	return t;
}


int robust_linkcp_iiNet(struct iiNet *net, struct CoupLink *cplk, int *lstate1) {
	long linesNum = cplk->lid_i12->linesNum;
	int *lstate = calloc(linesNum, sizeof(int));
	memcpy(lstate, lstate1, sizeof(int)*linesNum);

	int rob, maxi;
	int cplkinRobustNum = -1, dk = -1;
	while (cplkinRobustNum && dk) {
		//for (i = 0; i < cplk->lid_i12->linesNum; ++i) {
		//	if (lstate[i] != 0) {
		//		printf("%d\t%d\n", i, lstate[i]);
		//	}
		//}
		maxi = robust_iiNet(net, &rob);
		cplkinRobustNum = get_cplk_in_maxRobust(maxi, net, lstate, cplk->i12_lid);
		//printf("edgesNum: %ld\n", net->edgesNum);
		//printf("maxi: %d, rob: %d, cplkinRobustNum: %d\n", maxi, rob, cplkinRobustNum);
		dk = delete_linkcp_iiNet(net, cplk, lstate);
		//printf("cplk in Robust is %d, delete cplk is %d, now edgesNum is %ld\n", cplkinRobustNum, dk, net->edgesNum);
	}
	//printf("/********************************************************************************************************/\n");

	free(lstate);
	return rob;
}


void set_2_lstate(int *lstate, struct iiNet *net, int subthisid, struct LineFile *lf, int *fg, int **gid_lids, int *gidCount) {
	long i;
	int j;
	for (i = 0; i < lf->linesNum; ++i) {
		if (lstate[i] != 2) {
			if (lf->i1[i] == subthisid || lf->i2[i] == subthisid) {
				lstate[i] = 2;
				int gid = fg[i];
				for (j = 0; j < gidCount[gid]; ++j) {
					int lid = gid_lids[gid][j];
					lstate[lid] = 2;
					if (lf->i1[lid] != subthisid && lf->i2[lid] != subthisid) {
						delete_link_iiNet(net, lf->i1[lid], lf->i2[lid]);
					}
				}
			}
		}
	}
}

int main(int argc, char **argv)
{
	print_time();

	/********************************************************************************************************/
	int es, N, seed, MM0, kor;
	double q;
	int coupNum;
	robust_argc_argv(argc, argv, &es, &N, &seed, &MM0, &kor, &q, &coupNum);
	/********************************************************************************************************/

	/***************create net; lf, lcpnet, fg; maxg, lcpCount, lcp, .***************************************/
	struct LineFile *lf = robust_ER_or_SF(es, N, seed, MM0);
	struct iiNet *net = create_iiNet(lf);

	return 0;
	//verify_duplicatePairs_iiNet(net);
	//verify_fullyConnected_iiNet(net);
	int *gidCount;
	int *fg = robust_get_fg_2(lf, q, &gidCount);
	int maxg, **lcp;
	robust_get_linkcp(lf, fg, &maxg, gidCount, &lcp);
	int i;
	int j;
	int *i3 = malloc(lf->linesNum * sizeof(int));
	for (j = 0; j < lf->linesNum; ++j) {
		i3[j] = j;	
	}
	lf->i3 = i3;
	lf->filename = "couplinglinks";
	struct i3Net *lcpnet = create_i3Net(lf);
	//print_i3Net(lcpnet, "i3net");
	struct CoupLink *cplk = malloc(sizeof(struct CoupLink));
	cplk->lid_i12 = lf;
	cplk->i12_lid = lcpnet;
	cplk->lid_gid = fg;
	cplk->gidMax = maxg;
	cplk->gidCount = gidCount;
	cplk->gid_lids = lcp;
	/********************************************************************************************************/
	
	/********************************************************************************************************/
	//int robust = robust_linkcp_iiNet(net, cplk);
	//printf("result:\t%d\tQ(p):\t%f\n", net->maxId + 1, (double)robust/(net->maxId + 1));
	/********************************************************************************************************/

	/********************************************************************************************************/
	int *dl = robust_deletelist(net, kor);
	int *lstate = calloc(cplk->lid_i12->linesNum, sizeof(int));
	for (i = 0; i < net->maxId; ++i) {
		int subthisid = dl[i];
		long count_subthisid = net->count[subthisid];
		set_2_lstate(lstate, net, subthisid, cplk->lid_i12, cplk->lid_gid, cplk->gid_lids, cplk->gidCount);
		//printf("%d\n", subthisid);
		//for (j = 0; j < cplk->lid_i12->linesNum; ++j) {
		//	if (lstate[j] == 2) 
		//	printf("%d\t%d\t%d\t%d\t%d\n", j, lstate[j], lf->i1[j], lf->i2[j], fg[j]);
		//}
		delete_node_iiNet(net, subthisid);
		//printf("idnum: %d\n", net->idNum);
		int robust = robust_linkcp_iiNet(net, cplk, lstate);
		printf("result:CQ\tp:\t%f\tsubthisid:\t%d\tcount:\t%ld\tQ(p):\t%f\tC(p):\t%f\t%d\n", (double)(i+1)/(net->maxId + 1), subthisid, count_subthisid, (double)robust/(net->maxId + 1), (double)(net->maxId - i -robust)/(net->maxId - i), robust);
	}
	free(dl);
	/********************************************************************************************************/

	/********************************************************************************************************/
	free(lstate);
	free_iiNet(net);
	free_i3Net(lcpnet);
	free_LineFile(lf);
	free(fg);
	free(gidCount);
	for (i = 0; i < maxg + 1; ++i) {
		free(lcp[i]);
	}
	free(lcp);
	free(cplk);
	/********************************************************************************************************/

	print_time();
	return 0;
}
