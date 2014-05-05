#include "base.h"
#include "linefile.h"
#include "iinet.h"
#include "dataset.h"
#include "sort.h"
#include "mtprand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	print_time();
	int N, seed, MM0;
	if (argc == 1) {
		N = 10000;
		seed = 1;
		MM0 = 2;
	}
	else if (argc == 4) {
		char *p;
		N = strtol(argv[1], &p, 10);
		seed = strtol(argv[2], &p, 10);
		MM0 = strtol(argv[3], &p, 10);
	}
	else {
		isError("wrong arg");
	}

	//struct LineFile *lf = SF_DS(N, seed, MM0);
	struct LineFile *lf = ER_DS(N, seed);
	struct iiNet *net = create_iiNet(lf);
	free_LineFile(lf);
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
	long *count = calloc(net->maxId + 1, sizeof(long));
	memcpy(count, net->count, (net->maxId + 1)*sizeof(long));
	int *id = malloc((net->maxId + 1)*sizeof(int));
	for (i = 0; i < net->maxId + 1; ++i) {
		id[i] = i;
	}
	//qsort_li_desc(count, 0, net->maxId, id);
	//for (i = 0; i < net->maxId + 1; ++i) {
	int kk = net->maxId + 1;
	i = 1;
	while (kk > 0) {
		//int subthisid = id[i];
		int tmp = get_i31_MTPR()%kk;
		int subthisid = id[tmp];
		id[tmp] = id[--kk];
		//printf("%d\t%ld\n", id[i], count[i]);
		delete_node_iiNet(net, subthisid);
		int a = robust_iiNet(net);
		//if (a > net->idNum) {
			printf("result: CQ:\t%d\t%d\t%ld\t%d\t%d\t%d\n", i++, id[subthisid], count[subthisid], a, net->maxId + 1, net->idNum);
		//}
	}
	free(count);
	free(id);
	free_iiNet(net);
	print_time();
	return 0;
}
