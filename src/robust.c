#include "base.h"
#include "mtprand.h"
#include "dataset.h"
#include "sort.h"
#include "robust.h"
#include <stdlib.h>
#include <string.h>

struct LineFile *robust_ER_or_SF(int es, int N, int seed, int MM0) {
	struct LineFile *lf;
	if (es == 1) {
		lf = ER_DS(N, seed);
	}
	else if (es == 2) {
		lf = SF_DS(N, seed, MM0);
	}
	else {
		isError("ER or SF");
	}
	return lf;
}

void robust_argc_argv(int argc, char **argv, int *es, int *N, int *seed, int *MM0, int *kor, double *q) {
	if (argc == 1) {
		*es = 2;
		*N = 10000;
		*seed = 1;
		*MM0 = 5;
		*kor = 1;
		*q = 0.5;
	}
	else if (argc == 2) {
		char *p;
		*es = 2;
		*N = 10000;
		*seed = 1;
		*MM0 = 5;
		*kor = 2;
		*q = strtod(argv[1], &p);
	}
	else if (argc == 7) {
		char *p;
		*es = strtol(argv[1], &p, 10);
		*N = strtol(argv[2], &p, 10);
		*seed = strtol(argv[3], &p, 10);
		*MM0 = strtol(argv[4], &p, 10);
		*kor = strtol(argv[5], &p, 10);
		*q = strtod(argv[6], &p);
	}
	else {
		isError("wrong arg");
	}
}

int *robust_deletelist(struct iiNet *net, int kor) {
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
		isError("robust_deletelist");
	}
	return id;
}

