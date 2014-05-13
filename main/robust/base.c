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

int main(int argc, char **argv)
{
	print_time();

	int es, N, seed, MM0, kor;
	double q;
	int coupNum;
	robust_argc_argv(argc, argv, &es, &N, &seed, &MM0, &kor, &q, &coupNum);

	struct LineFile *lf = robust_ER_or_SF(es, N, seed, MM0);
	struct iiNet *net = create_iiNet(lf);
	free_LineFile(lf);

	long *dd = degree_distribution_iiNet(net);
	int i;
	for (i = 0; i < net->countMax + 1; ++i) {
		printf("result:\tdegree\t%d\t%ld\n", i, dd[i]);
	}
	free(dd);

	int robust;
	robust_iiNet(net, &robust);
	printf("result:CQ\tp:\t%f\tsubthisid:\t%d\tcount:\t%ld\t%d\tQ(p):\t%f\tC(p):\t%f\n", 0.0, -1, -1L, net->maxId + 1, (double)robust/(net->maxId + 1), (double)(net->maxId + 1 -robust)/(net->maxId + 1));

	int *dl = robust_deletelist(net, kor);
	for (i = 0; i < net->maxId; ++i) {
		int subthisid = dl[i];
		long count_subthisid = net->count[subthisid];
		delete_node_iiNet(net, subthisid);
		int robust;
		robust_iiNet(net, &robust);
		printf("result:CQ\tp:\t%f\tsubthisid:\t%d\tcount:\t%ld\t%d\tQ(p):\t%f\tC(p):\t%f\n", (double)(i+1)/(net->maxId + 1), subthisid, count_subthisid, net->maxId + 1, (double)robust/(net->maxId + 1), (double)(net->maxId - i -robust)/(net->maxId - i));
	}
	free(dl);

	free_iiNet(net);

	print_time();
	return 0;
}
