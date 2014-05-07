//./robust-base 2 12345 1 20 1
#include "base.h"
#include "linefile.h"
#include "iinet.h"
#include "dataset.h"
#include "sort.h"
#include "mtprand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "robust.h"

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
		//printf("%d\t%ld\t%d\n", i, lcpCount[i], maxg);
		lcp[i] = malloc(lcpCount[i]*sizeof(int));
	}
	int *tmpCount = calloc(maxg + 1, sizeof(int));
	for (i = 0; i < rlNum; ++i) {
		//printf("%ld\t%d\t%ld\t%ld\n", i, fg[i], lcpCount[fg[i]], tmpCount[fg[i]]);
		lcp[fg[i]][(tmpCount[fg[i]])++] = i;
	}
	free(tmpCount);
	*lcpCounts = lcpCount;
	*lcps = lcp;	
	*maxgs = maxg;
}

static int extract_backbone_iiNet(int nid, struct iiNet *net, char *fg, int *left, int *right) {
	if (fg[nid] == 1) isError("extract_backbone_iiNet");
	int lN = 0, rN = 0;
	left[lN++] = nid;
	fg[nid] = 1;
	int conn = 1;
	int i;
	long j;
	while(lN) {
		rN = 0;
		for (i = 0; i < lN; ++i) {
			int id = left[i];
			for (j = 0; j < net->count[id]; ++j) {
				int neigh = net->edges[id][j];
				if (fg[neigh] == 0) {
					fg[neigh] = 1;
					++conn;
					right[rN++] = neigh;
				}
			}
		}
		int *tmp = left;
		left = right;
		right = tmp;
		lN = rN;
	}
	return conn;
}

double robust_linkcp_iiNet(struct iiNet *net, struct iiNet *lcpnet, int **lcp) {
	int N = net->idNum;
	int maxru = 0;
	int already = 0;

	char *fg = calloc(net->maxId + 1, sizeof(char));
	int *left = malloc((net->maxId + 1) * sizeof(int));
	int *right = malloc((net->maxId + 1) * sizeof(int));

	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		if (fg[i] == 0 && net->count[i]) {
			int conn = extract_backbone_iiNet(i, net, fg, left, right);
			already += conn;
			maxru = imax(conn, maxru);
			if (maxru >= N-already) break;
		}
	}

	free(fg);
	free(left);
	free(right);
	return maxru;
	return 0.0;
}

int main(int argc, char **argv)
{
	print_time();

	int es, N, seed, MM0, kor;
	double q;
	robust_argc_argv(argc, argv, &es, &N, &seed, &MM0, &kor, &q);

	struct LineFile *lf = robust_ER_or_SF(es, N, seed, MM0);
	struct iiNet *net = create_iiNet(lf);
	int *fg = robust_get_fg(lf, q);
	int maxg, *lcpCount, **lcp;
	robust_get_linkcp(lf, fg, &maxg, &lcpCount, &lcp);
	struct iiNet *lcpnet = create_iiNet(lf);


	int *dl = robust_deletelist(net, kor);
	int i;
	for (i = 0; i < net->maxId; ++i) {
		int subthisid = dl[i];
		long count_subthisid = net->count[subthisid];
		delete_node_iiNet(net, subthisid);
		int robust = robust_linkcp_iiNet(net, lcpnet, lcp);
		printf("result:CQ\tp:\t%f\tsubthisid:\t%d\tcount:\t%ld\t%d\tQ(p):\t%f\tC(p):\t%f\n", (double)(i+1)/(net->maxId + 1), subthisid, count_subthisid, net->maxId + 1, (double)robust/(net->maxId + 1), (double)(net->maxId - i -robust)/(net->maxId - i));
	}
	free(dl);

	/*
	   free_LineFile(lf);

	   long *dd = degree_distribution_iiNet(net);
	   int i;
	   for (i = 0; i < net->countMax + 1; ++i) {
	   printf("result:\tdegree\t%d\t%ld\n", i, dd[i]);
	   }
	   free(dd);

	   int robust = robust_iiNet(net);
	   printf("result:CQ\tp:\t%f\tsubthisid:\t%d\tcount:\t%ld\t%d\tQ(p):\t%f\tC(p):\t%f\n", 0.0, -1, -1L, net->maxId + 1, (double)robust/(net->maxId + 1), (double)(net->maxId + 1 -robust)/(net->maxId + 1));


	   free_iiNet(net);
	   */

	free_iiNet(net);
	free_iiNet(lcpnet);
	free_LineFile(lf);
	free(fg);
	free(lcpCount);
	for (i = 0; i < maxg + 1; ++i) {
		free(lcp[i]);
	}
	free(lcp);

	print_time();
	return 0;
}
