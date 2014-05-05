#include "base.h"
#include "linefile.h"
#include "iinet.h"
#include "dataset.h"
#include "sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	print_time();
	struct LineFile *lf = SF_DS(10000, 1);
	struct iiNet *net = create_iiNet(lf);
	//print_iiNet(net, "tt");
	int ab = robust_iiNet(net);
	printf("%d\t%d\t%d\t%d\n", -1, ab, net->maxId + 1, net->idNum);
	long *count = calloc(net->maxId + 1, sizeof(long));
	memcpy(count, net->count, (net->maxId + 1)*sizeof(long));
	int *id = malloc((net->maxId + 1)*sizeof(int));
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		id[i] = i;
	}
	qsort_li_desc(count, 0, net->maxId, id);
	for (i = 0; i < net->maxId + 1; ++i) {
		int subthisid = id[i];
		printf("%d\t%ld\n", id[i], count[i]);
		delete_node_iiNet(net, subthisid);
		int a = robust_iiNet(net);
		//if (a > net->idNum) {
			printf("%d\t%d\t%d\t%d\t%d\n", i, id[i], a, net->maxId + 1, net->idNum);
		//}
	}
	print_time();
	return 0;
}
