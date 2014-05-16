#include "base.h"
#include "mtprand.h"
#include "dataset.h"
#include "sort.h"
#include "robust.h"
#include "i3net.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>

struct LineFile *robust_ER_or_SF(int es, int N, int seed, int MM0) {
	print2l("%s =>> begin......\n", __func__);
	struct LineFile *lf;
	if (es == 1) {
		lf = ER_DS(N, seed);
	}
	else if (es == 2) {
		lf = SF_DS(N, seed, MM0);
	}
	else {
		isError("%s =>> wrong network type\n", __func__);
	}
	print2l("%s =>> ......end.\n", __func__);
	return lf;
}

void robust_argc_argv(int argc, char **argv, int *es, int *N, int *seed, int *MM0, int *kor, double *q, int *algorithm_type, int *pairsNum) {
	print2l("%s =>> begin......\n", __func__);
	if (argc == 1) {
		*es = 2;
		*N = 10000;
		*seed = 1;
		*MM0 = 5;
		*kor = 2;
		*q = 0.5;
		*algorithm_type = 2;
		*pairsNum = 3;
	}
	else if (argc == 2) {
		char *p;
		*es = 2;
		*N = 10000;
		*seed = 1;
		*MM0 = 5;
		*kor = 2;
		*q = strtod(argv[1], &p);
		*algorithm_type = 1;
		*pairsNum = 3;
	}
	else if (argc == 9) {
		char *p;
		*es = strtol(argv[1], &p, 10);
		*N = strtol(argv[2], &p, 10);
		*seed = strtol(argv[3], &p, 10);
		*MM0 = strtol(argv[4], &p, 10);
		*kor = strtol(argv[5], &p, 10);
		*q = strtod(argv[6], &p);
		*algorithm_type =  strtol(argv[7], &p, 10);
		*pairsNum =  strtol(argv[8], &p, 10);
	}
	else {
		isError("%s =>> wrong arg.\n", __func__);
	}
	print2l("%s =>> ......end.\n", __func__);
}

int *robust_create_deletelist(struct iiNet *net, int kor) {
	print2l("%s =>> begin......\n", __func__);
	int *id, i;
	if (kor == 1) { //k max delete first.
		long *count = calloc(net->maxId + 1, sizeof(long));
		id = malloc((net->maxId + 1)*sizeof(int));
		memcpy(count, net->count, (net->maxId + 1)*sizeof(long));
		for (i = 0; i < net->maxId + 1; ++i) {
			id[i] = i;
		}
		qsort_li_desc(count, 0, net->maxId, id);
		free(count);
	}
	else if (kor == 2) { // random delete.
		id = malloc((net->maxId + 1)*sizeof(int));
		for (i = 0; i < net->maxId + 1; ++i) {
			id[i] = i;
		}
		int kk = net->maxId + 1;
		while (kk > 0) {
			int tmp = get_i31_MTPR()%kk;
			int subthisid = id[tmp];
			id[tmp] = id[kk-1];
			id[kk-1] = subthisid;
			--kk;
		}
	}
	else {
		isError("%s =>> wrong deletelist type.\n", __func__);
	}
	print2l("%s =>> ......end.\n", __func__);
	return id;
}

static int *robust_set_each_link_group_algorithm01(struct LineFile *lf, double q, int **gidCounts) {
	print4l("%s =>> begin......\n", __func__);
	if (q<0 || q>1) isError("%s =>> invalid possiblity q value.\n", __func__);
	
	long cplkNum = (long)(q*lf->linesNum);
	print5l("%s =>> number of coupling links will be %ld.\n", __func__, cplkNum);
	
	int * lid_gid = smalloc(lf->linesNum * sizeof(int));
	int *gidCount = scalloc(cplkNum/2, sizeof(int));
	int gid = 0;
	long i;
	for (i = 0; i < lf->linesNum; ++i) {
		lid_gid[i] = -1;
	}
	long cpNum = 0;
	while (cpNum < cplkNum) {
		int l1 = get_i31_MTPR()%(lf->linesNum);
		int l2 = get_i31_MTPR()%(lf->linesNum);
		if (l1 == l2) continue;
		if (lid_gid[l1] == -1 && lid_gid[l2] == -1) {
			lid_gid[l1] = lid_gid[l2] = gid;
			gidCount[gid] += 2;
			cpNum += 2;
			++gid;
		}
		else if (lid_gid[l1] == -1 && lid_gid[l2] != -1) {
			lid_gid[l1] = lid_gid[l2];
			++gidCount[lid_gid[l2]];
			++cpNum;
		}
		else if (lid_gid[l1] != -1 && lid_gid[l2] == -1) {
			lid_gid[l2] = lid_gid[l1];
			++gidCount[lid_gid[l1]];
			++cpNum;
		}
		else if (lid_gid[l1] == lid_gid[l2]) {
			continue;
		}
		else {
			int sgid, bgid;
			if (gidCount[lid_gid[l1]] > gidCount[lid_gid[l2]]) {
				sgid = lid_gid[l2];
				bgid = lid_gid[l1];
			}
			else {
				sgid = lid_gid[l1];
				bgid = lid_gid[l2];
			}
			for (i = 0; i < lf->linesNum; ++i) {
				if (lid_gid[i] == sgid) {
					lid_gid[i] = bgid;
					--gidCount[sgid];
					++gidCount[bgid];
					if (gidCount[sgid] == 0) {
						break;
					}
				}
			}
			assert(gidCount[sgid] == 0);
		}
	}
		
	*gidCounts = gidCount;
	print4l("%s =>> ......end.\n", __func__);
	return lid_gid;
}

static int *robust_set_each_link_group_algorithm02(struct LineFile *lf, double q, int **gidCounts, int pairsNum) {
	print4l("%s =>> begin......\n", __func__);
	if (q<0 || q>1 || pairsNum < 2) isError("%s =>> invalid possiblity q value or pairsNum.\n", __func__);
	
	long cplkNum = (long)(q*lf->linesNum);
	print5l("%s =>> number of coupling links will be %ld.\n", __func__, cplkNum);
	
	int * lid_gid = smalloc(lf->linesNum * sizeof(int));
	int *gidCount = scalloc(cplkNum/2, sizeof(int));
	int gid = 0;
	long i;
	int *clean= smalloc(lf->linesNum * sizeof(int));
	for (i = 0; i < lf->linesNum; ++i) {
		lid_gid[i] = -1;
		clean[i] = i;
	}
	long cleanNum = lf->linesNum;

	long cpNum = 0;
	while (cpNum < cplkNum - pairsNum) {
		int pn = 0;
		while (pn < pairsNum) {
			int index = get_i31_MTPR()%cleanNum;
			int lid = clean[index];
			lid_gid[lid] = gid;
			clean[index] = clean[--cleanNum];
			++cpNum;
			++pn;
		}
		gidCount[gid] = pairsNum;
		++gid;
	}
		
	free(clean);
	*gidCounts = gidCount;
	print4l("%s =>> ......end.\n", __func__);
	return lid_gid;
}

static void robust_set_lidi12_lidgid_gidMax_gidlids(int *gidCount, struct LineFile *lid_i12, int *lid_gid, int *gidMaxs, int ***gid_lidss, int *gidCountMaxs, int *gidCountMins) {
	long rlNum = 0;
	long i;
	int gidMax = -1;
	for (i = 0; i < lid_i12->linesNum; ++i) {
		if (lid_gid[i] == -1) continue;
		gidMax = imax(gidMax, lid_gid[i]);
		lid_i12->i1[rlNum] = lid_i12->i1[i];
		lid_i12->i2[rlNum] = lid_i12->i2[i];
		lid_gid[rlNum] = lid_gid[i];
		++rlNum;
	}
	lid_i12->linesNum = rlNum;
	//for (i = 0; i < rlNum; ++i) {
	//	//printf("i,lid_gid[i]: %d\t%d\n", i, lid_gid[i]);
	//}
	int gidCountMax = -1, gidCountMin = INT_MAX;
	int **gid_lids = smalloc((gidMax + 1)*sizeof(int *));
	for (i = 0; i < gidMax + 1; ++i) {
		gidCountMax = imax(gidCountMax, gidCount[i]);
		gidCountMin = imin(gidCountMin, gidCount[i]);
		gid_lids[i] = smalloc(gidCount[i]*sizeof(int));
	}
	int *tmpCount = scalloc(gidMax + 1, sizeof(int));
	int *i3 = smalloc(rlNum * sizeof(int));
	for (i = 0; i < rlNum; ++i) {
		int gid = lid_gid[i];
		gid_lids[gid][(tmpCount[gid])++] = i;
		i3[i] = i;
	}
	lid_i12->i3 = i3;
	lid_i12->filename = "couplinglinks";
	free(tmpCount);
	*gid_lidss = gid_lids;	
	*gidMaxs = gidMax;
	*gidCountMaxs = gidCountMax;
	*gidCountMins = gidCountMin;
}

static int *robust_set_each_link_group(struct LineFile *lid_i12, double q, int **gidCount, int algorithm_type, int pairsNum) {
	if (algorithm_type == 1) {
		return robust_set_each_link_group_algorithm01(lid_i12, q, gidCount);
	}	
	if (algorithm_type == 2) {
		return robust_set_each_link_group_algorithm02(lid_i12, q, gidCount, pairsNum);
	}
	return NULL;
}


struct CoupLink * robust_get_cplk(struct LineFile *lid_i12, double q, int algorithm_type, int pairsNum) {
	print2l("%s =>> begin......\n", __func__);
	int *gidCount;
	int *lid_gid = robust_set_each_link_group(lid_i12, q, &gidCount, algorithm_type, pairsNum);
	int gidMax, **gid_lids, gidCountMax, gidCountMin;
	robust_set_lidi12_lidgid_gidMax_gidlids(gidCount, lid_i12, lid_gid, &gidMax, &gid_lids, &gidCountMax, &gidCountMin);

	struct i3Net *i12_lid = create_i3Net(lid_i12);
	print3l("%s =>> Max: %d, Min: %d, idNum: %d, edgesNum: %ld, countMax: %ld, countMin: %ld\n", __func__, i12_lid->maxId, i12_lid->minId, i12_lid->idNum, i12_lid->edgesNum, i12_lid->countMax, i12_lid->countMin);
	struct CoupLink *cplk = smalloc(sizeof(struct CoupLink));
	cplk->i12_lid = i12_lid;
	cplk->lid_i12 = lid_i12;
	cplk->lid_gid = lid_gid;
	cplk->gidMax = gidMax;
	cplk->gidCountMax = gidCountMax;
	cplk->gidCountMin = gidCountMin;
	cplk->gidCount = gidCount;
	cplk->gid_lids = gid_lids;
	print2l("%s =>> ......end.\n", __func__);
	return cplk;
}

void free_CPLK(struct CoupLink *cplk) {
	free_i3Net(cplk->i12_lid);
	free(cplk->lid_gid);
	free(cplk->gidCount);
	int i;
	for (i = 0; i < cplk->gidMax + 1; ++i) {
		free(cplk->gid_lids[i]);
	}
	free(cplk->gid_lids);
	free(cplk);
}

