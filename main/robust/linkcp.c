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

static int find_lid_from_i12(int id, int neigh, struct i3Net *i12_lid) {
	if (id > i12_lid->maxId) return -1;
	long i;
	for (i = 0; i < i12_lid->count[id]; ++i) {
		if (i12_lid->edges[id][i] == neigh) {
			return i12_lid->i3[id][i];
		}
	}
	return -1;
}

static int get_cplk_set_lstate_in_maxRobust(int nid, struct iiNet *net, int *lstate, struct i3Net *i12_lid) {
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

int robust_delete_cross_linkcp(struct iiNet *net, struct CoupLink *cplk, int *lstate) {

	struct LineFile *lf = cplk->lid_i12;
	int *lid_gid = cplk->lid_gid;
	int **gid_lids = cplk->gid_lids;
	int *gidCount = cplk->gidCount;
	
	int t=0;
	int i, j;
	for (i = 0; i < lf->linesNum; ++i) {
		if (lstate[i] == 1) {
			int ssi = 0;
			int gid = lid_gid[i];
			for (j = 0; j < gidCount[gid]; ++j) {
				int lid = gid_lids[gid][j];
				if (lstate[lid] == 0 ) {
					assert(net->count[lf->i1[lid]] != 0);
				    assert(net->count[lf->i2[lid]] != 0);
					ssi = 1;
					break;
				}
			}
			if (ssi == 1) {
				for (j = 0; j < gidCount[gid]; ++j) {
					int lid = gid_lids[gid][j];
					assert(lstate[lid] != 2);
					lstate[lid] = 2;
					int x = delete_link_iiNet(net, lf->i1[lid], lf->i2[lid]);
					assert(x == 1);
					t += x;
				}
			}
			else {
				for (j = 0; j < gidCount[gid]; ++j) {
					int lid = gid_lids[gid][j];
					lstate[lid] = 0;
				}
			}
		}
	}
	return t;
}

static int linkcp_iiNet(struct iiNet *net, struct CoupLink *cplk, int *lstate1) {
	print4l("%s =>> begin......\n", __func__);
	long linesNum = cplk->lid_i12->linesNum;
	int *lstate = calloc(linesNum, sizeof(int));
	memcpy(lstate, lstate1, sizeof(int)*linesNum);

	int nodesNuminRobust, oneidinRobust;
	int cplkinRobustNum = -1, dk = -1;
	while (cplkinRobustNum && dk) {
		oneidinRobust= robust_iiNet(net, &nodesNuminRobust);
		cplkinRobustNum = get_cplk_set_lstate_in_maxRobust(oneidinRobust, net, lstate, cplk->i12_lid);
		dk = robust_delete_cross_linkcp(net, cplk, lstate);
		print1l("%s =>> cross linkcp deletion: %d, (cplkinRobustNum: %d, nodesNuminRobust: %d).\n", __func__, dk, cplkinRobustNum, nodesNuminRobust);
	}

	free(lstate);
	print4l("%s =>> ......end.\n", __func__);
	return nodesNuminRobust;
}


static int robust_sign_deleted_cplk(int *lstate, struct iiNet *net, int subthisid, struct LineFile *i12_lid, int *lid_gid, int **gid_lids, int *gidCount) {
	long i, j;
	int t = 0;
	for (i = 0; i < i12_lid->linesNum; ++i) {
		if (lstate[i] != 2) {
			if (i12_lid->i1[i] == subthisid || i12_lid->i2[i] == subthisid) {
				lstate[i] = 2;
				int gid = lid_gid[i];
				for (j = 0; j < gidCount[gid]; ++j) {
					int lid = gid_lids[gid][j];
					lstate[lid] = 2;
					int x = delete_link_iiNet(net, i12_lid->i1[lid], i12_lid->i2[lid]);
					//assert(x == 1);
					t += x;
				}
			}
		}
	}
	return t;
}

static char *ES_S[] = {"ER network", "SF network"};
static char *KOR_S[] = {"larger degree node delte list", "random delete list"};

int main(int argc, char **argv)
{
	print_time();
	(void)ES_S;
	(void)KOR_S;

	/********************************************************************************************************/
	int es, N, seed, MM0, kor;
	double q;
	int algorithm_type, pairsNum;
	robust_argc_argv(argc, argv, &es, &N, &seed, &MM0, &kor, &q, &algorithm_type, &pairsNum);
	print1l("%s =>> nodes Num: %d, random seed: %d, MM0: %d, q: %f, coupNum: %d.\n", __func__, N, seed, MM0, q, coupNum);
	/********************************************************************************************************/

	/***************create net & cplk.***************************************/
	struct LineFile *lf = robust_ER_or_SF(es, N, seed, MM0);
	struct iiNet *net = create_iiNet(lf);
	print1l("%s =>> network type: %s\n", __func__, ES_S[es-1]);
	print1l("%s =>> create network, Max: %d, Min: %d, idNum: %d, edgesNum: %ld, countMax: %ld, countMin: %ld\n", __func__, net->maxId, net->minId, net->idNum, net->edgesNum, net->countMax, net->countMin);

	struct CoupLink *cplk = robust_get_cplk(lf, q, algorithm_type, pairsNum);
	print1l("%s =>> create CPLKs, type: %d, pairsNum: %d, q: %f, cplkNum: %ld, netlkNum: %ld, cplkNodeNum: %d, netnodeNum: %d, gidMax: %d, gidCountMax: %d, gidCountMin: %d.\n", __func__, algorithm_type, pairsNum, q, cplk->lid_i12->linesNum, net->edgesNum, cplk->i12_lid->idNum, net->idNum, cplk->gidMax, cplk->gidCountMax, cplk->gidCountMin);
	/********************************************************************************************************/
	
	/********************************************************************************************************/
	int *dl = robust_create_deletelist(net, kor);
	print1l("%s =>> deletelist type: %s\n", __func__, KOR_S[kor-1]);
	int *lstate = scalloc(cplk->lid_i12->linesNum, sizeof(int));
	int i;
	for (i = 0; i < net->maxId; ++i) {
		int subthisid = dl[i];
		long count_subthisid = net->count[subthisid];
		int t = robust_sign_deleted_cplk(lstate, net, subthisid, cplk->lid_i12, cplk->lid_gid, cplk->gid_lids, cplk->gidCount);
		(void)t;
		print1l("%s =>> node \"%d\" deletion, cplk deletion: %d, nodelk deletion: %ld, now edgesNum is %ld.\n", __func__, subthisid, t, net->count[subthisid], net->edgesNum - net->count[subthisid]);
		delete_node_iiNet(net, subthisid);
		int robust = linkcp_iiNet(net, cplk, lstate);
		printf("result:CQ\tp:\t%f\tsubthisid:\t%d\tcount:\t%ld\tQ(p):\t%f\tC(p):\t%f\t%d\n", (double)(i+1)/(net->maxId + 1), subthisid, count_subthisid, (double)robust/(net->maxId + 1), (double)(net->maxId - i -robust)/(net->maxId - i), robust);
	}
	free(dl);
	/********************************************************************************************************/

	/********************************************************************************************************/
	free(lstate);
	free_iiNet(net);
	free_LineFile(lf);
	free_CPLK(cplk);
	/********************************************************************************************************/

	print_time();
	return 0;
}
