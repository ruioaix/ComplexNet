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
	robust_argc_argv(argc, argv, &es, &N, &seed, &MM0, &kor);

	struct LineFile *lf = robust_ER_or_SF(es, N, seed, MM0);
	struct iiNet *net = create_iiNet(lf);
	free_LineFile(lf);

	long *dd = degree_distribution_iiNet(net);
	int i;
	for (i = 0; i < net->countMax + 1; ++i) {
		printf("result:\t%d\t%d\t%ld\n", 1, i, dd[i]);
	}
	free(dd);

	int *dl = robust_deletelist(net, kor);
	for (i = 0; i < net->maxId + 1; ++i) {
		int subthisid = dl[i];
		long count_subthisid = net->count[subthisid];
		delete_node_iiNet(net, subthisid);
		int robust = robust_iiNet(net);
		printf("result:\t%d\t%d\tsubthisid:\t%d\tcount:\t%ld\t%d\trobust:\t%d\tidNum:\t%d\tidNum-robust:\t%d\tmaxid+1-robust:\t%dmaxId+1-idNum:\t%d\n", 2, i, subthisid, count_subthisid, net->maxId + 1, robust, net->idNum, net->idNum-robust, net->maxId + 1 - robust, net->maxId + 1 - net->idNum);
	}
	free(dl);

	free_iiNet(net);

	print_time();
	return 0;
}
