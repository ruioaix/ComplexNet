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

void robust_get_linkcp(struct LineFile *lf, int *fg, int *maxgs, int **lcpCounts, int ***lcps) {
	long rlNum = 0;
	long i;
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
	int *lcpCount = calloc(maxg + 1, sizeof(int));
	for (i = 0; i < rlNum; ++i) {
		++(lcpCount[fg[i]]);
	}
	int **lcp = malloc((maxg + 1)*sizeof(int *));
	for (i = 0; i < maxg + 1; ++i) {
		lcp[i] = malloc(lcpCount[i]*sizeof(int));
	}
	int *tmpCount = calloc(maxg + 1, sizeof(int));
	for (i = 0; i < rlNum; ++i) {
		lcp[fg[i]][(tmpCount[fg[i]])++] = i;
	}
	free(tmpCount);
	*lcpCounts = lcpCount;
	*lcps = lcp;	
	*maxgs = maxg;
}

int find_lineNum_LF(int id, int neigh, struct i3Net *lcpnet) {
	int i;
	for (i = 0; i < lcpnet->count[id]; ++i) {
		if (lcpnet->edges[id][i] == neigh) {
			return lcpnet->i3[id][i];
		}
	}
	return -1;
}

static int check_maxrobust_iiNet(int nid, struct iiNet *net, struct i3Net *lcpnet, int *lfg, struct LineFile*lf) {
	char *fg = calloc(net->maxId + 1, sizeof(char));
	int *left = malloc((net->maxId + 1) * sizeof(int));
	int *right = malloc((net->maxId + 1) * sizeof(int));

	int lk = 0;

	if (fg[nid] == 1) isError("extract_backbone_iiNet");
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
				int lineNum = find_lineNum_LF(id, neigh, lcpnet);
				if (lineNum != -1) {
					if (lfg[lineNum] == 0) {
						lfg[lineNum] = 1;
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

int delete_linkcp_iiNet(struct iiNet *net, struct LineFile *lf, int *fg, int maxg, int *lcpCount, int **lcp, int *lfg) {
	int i, j;
	int k = 0, t = 0;
	for (i = 0; i < lf->linesNum; ++i) {
		if (lfg[i] == 1) {
			int ssi = 0;
			for (j = 0; j < lcpCount[fg[i]]; ++j) {
				int id = lcp[fg[i]][j];
				if (lfg[id] == 0) {
					ssi = 1;
					break;
				}
			}
			if (ssi == 1) {
				for (j = 0; j < lcpCount[fg[i]]; ++j) {
					int id = lcp[fg[i]][j];
					if (lfg[id] != 2) {
						lfg[id] = 2;
						t += delete_link_iiNet(net, lf->i1[id], lf->i2[id]);
					}
				}
			}
		}
	}
	return k;
}


double robust_linkcp_iiNet(struct iiNet *net, struct CoupLink *cplk) {

	struct LineFile *lf = cplk->lid_i12;
	struct i3Net *lcpnet = cplk->i12_lid;
	int maxg = cplk->gidMax;
	int *lcpCount = cplk->gidCount;
	int *fg = cplk->lid_gid;
	int **lcp = cplk->gid_lids;

	int rob , dk = -1;
	int maxi = robust_iiNet(net, &rob);
	//get the coup links in the max robust.
	int *lfg = calloc(lf->linesNum, sizeof(int));
	int cn = check_maxrobust_iiNet(maxi, net, lcpnet, lfg, lf);
	while (cn && dk) {
		//get the max robust.
		dk = delete_linkcp_iiNet(net, lf, fg, maxg, lcpCount, lcp, lfg);
		maxi = robust_iiNet(net, &rob);
		free(lfg);
		lfg = calloc(lf->linesNum, sizeof(int));
		cn = check_maxrobust_iiNet(maxi, net, lcpnet, lfg, lf);
	}


	free(lfg);
	return rob;
}

int main(int argc, char **argv)
{
	print_time();

	/********************************************************************************************************/
	int es, N, seed, MM0, kor;
	double q;
	robust_argc_argv(argc, argv, &es, &N, &seed, &MM0, &kor, &q);
	/********************************************************************************************************/

	/***************create net; lf, lcpnet, fg; maxg, lcpCount, lcp, .***************************************/
	struct LineFile *lf = robust_ER_or_SF(es, N, seed, MM0);
	struct iiNet *net = create_iiNet(lf);
	int *fg = robust_get_fg(lf, q);
	int maxg, *lcpCount, **lcp;
	robust_get_linkcp(lf, fg, &maxg, &lcpCount, &lcp);
	int *i3 = malloc(lf->linesNum * sizeof(int));
	int j;
	for (j = 0; j < lf->linesNum; ++j) {
		i3[j] = j;	
	}
	lf->i3 = i3;
	lf->filename = "couplinglinks";
	struct i3Net *lcpnet = create_i3Net(lf);
	struct CoupLink *cplk = malloc(sizeof(struct CoupLink));
	cplk->lid_i12 = lf;
	cplk->i12_lid = lcpnet;
	cplk->lid_gid = fg;
	cplk->gidMax = maxg;
	cplk->gidCount = lcpCount;
	cplk->gid_lids = lcp;
	/********************************************************************************************************/
	
	/********************************************************************************************************/
	int robust = robust_linkcp_iiNet(net, cplk);
	printf("result:\t%d\tQ(p):\t%f\n", net->maxId + 1, (double)robust/(net->maxId + 1));
	/********************************************************************************************************/

	/********************************************************************************************************/
	int *dl = robust_deletelist(net, kor);
	int i;
	for (i = 0; i < net->maxId; ++i) {
		int subthisid = dl[i];
		long count_subthisid = net->count[subthisid];
		delete_node_iiNet(net, subthisid);
		int robust = robust_linkcp_iiNet(net, cplk);

		printf("result:CQ\tp:\t%f\tsubthisid:\t%d\tcount:\t%ld\tQ(p):\t%f\tC(p):\t%f\t%d\n", (double)(i+1)/(net->maxId + 1), subthisid, count_subthisid, (double)robust/(net->maxId + 1), (double)(net->maxId - i -robust)/(net->maxId - i), robust);
	}
	free(dl);
	/********************************************************************************************************/

	/********************************************************************************************************/
	free_iiNet(net);
	free_i3Net(lcpnet);
	free_LineFile(lf);
	free(fg);
	free(lcpCount);
	for (i = 0; i < maxg + 1; ++i) {
		free(lcp[i]);
	}
	free(lcp);
	free(cplk);
	/********************************************************************************************************/

	print_time();
	return 0;
}
