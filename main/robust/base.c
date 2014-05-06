#include "base.h"
#include "linefile.h"
#include "iinet.h"
#include "dataset.h"
#include "sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct LineFile *robust_ER_or_SF(int es, int N, int seed, int MM0) {
	struct LineFile *lf;
	if (es == 1) {
		lf = ER_DS(N, seed);
	}
	else if (es == 2) {
		lf = SF(N, seed, MM0);
	}
	else {
		isError("ER or SF");
	}
	return lf;
}

static void robust_argc_argv(int argc, char **argv, int *es, int *N, int *seed, int *MM0, int *kor) {
	if (argc == 1) {
		*es = 1;
		*N = 10000;
		*seed = 1;
		*MM0 = 2;
		*kor = 1;
	}
	else if (argc == 6) {
		char *p;
		*es = strtol(argv[1], &p, 10);
		*N = strtol(argv[2], &p, 10);
		*seed = strtol(argv[3], &p, 10);
		*MM0 = strtol(argv[4], &p, 10);
		*kor = strtol(argv[5], &p, 10);
	}
	else {
		isError("wrong arg");
	}
}

int *robust_deletelist(struct iiNet *net, int kor) {
	int *id;
	if (kor == 1) { //k max delete first.
		long *count = calloc(net->maxId + 1, sizeof(long));
		id = malloc((net->maxId + 1)*sizeof(int));
		memcpy(count, net->count, (net->maxId + 1)*sizeof(long));
		for (i = 0; i < net->maxId + 1; ++i) {
			id[i] = i;
		}
		qsort_li_desc(count, 0, net->maxId, id);
	}
}

int main(int argc, char **argv)
{
	print_time();

	int es, N, seed, MM0, kor;
	robust_argc_argv(argc, argv, &es, &N, &seed, &MM0, &kor);

	struct LineFile *lf = robust_ER_or_SF(es, N, seed, MM0);
	struct iiNet *net = create_iiNet(lf);
	free_LineFile(lf);

	robust_deletelist(net, count, id, kor);
	int i;
	long *cd = calloc(net->countMax + 1, sizeof(long));
	for (i = 0; i < net->maxId + 1; ++i) {
		cd[net->count[i]]++;
	}
	for (i = 0; i < net->countMax + 1; ++i) {
		printf("result: degree distribution:\t%d\t%ld\n", i, cd[i]);
	}
	free(cd);
	//print_iiNet(net, "tt");
	//int ab = robust_iiNet(net);
	//printf("%d\t%d\t%d\t%d\n", -1, ab, net->maxId + 1, net->idNum);
	for (i = 0; i < net->maxId + 1; ++i) {
		int subthisid = id[i];
		//printf("%d\t%ld\n", id[i], count[i]);
		delete_node_iiNet(net, subthisid);
		int a = robust_iiNet(net);
		//if (a > net->idNum) {
			printf("result: CQ:\t%d\t%d\t%ld\t%d\t%d\t%d\n", i, id[i], count[i], a, net->maxId + 1, net->idNum);
		//}
	}
	free(count);
	free(id);
	free_iiNet(net);
	print_time();
	return 0;
}
