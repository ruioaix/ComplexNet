#include "dataset.h"
#include "iinet.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	struct LineFile *lf = ER_DS(10000, 1);
	struct iiNet *net = create_iiNet(lf);
	int i;
	long *cd = calloc(net->countMax + 1, sizeof(long));
	for (i = 0; i < net->maxId + 1; ++i) {
		cd[net->count[i]]++;
	}
	for (i = 0; i < net->countMax + 1; ++i) {
		//printf("%d\t%ld\n", i, cd[i]);
	}
	verify_connectedness_iiNet(net);
	return 0;
}
