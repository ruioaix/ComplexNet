#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "base.h"
#include "iinet.h"
#include "spath.h"
#include "mtprand.h"
#include "assert.h"
#include "tdpotn.h"

double calculate_gini(struct iidNet *net, struct iiNet *base) {
	int i,j;
	int m,n;
	double diff = 0.0;
	double total = 0.0;
	for (i = 0; i < net->maxId + 1; ++i) {
		for (j = 0; j < net->count[i]; ++j) {
			if (i<net->edges[i][j]) {
				double x1 = net->d[i][j];	
				total += x1;
				for (m = 0; m < net->maxId + 1; ++m) {
					for (n = 0; n < net->count[m]; ++n) {
						if (m < net->edges[m][n]) {
							double x2 = net->d[m][n];
							diff += fabs(x1 - x2);
						}
					}
				}
			}
		}
	}
	double E = (double)net->edgesNum;
	double G = diff/(2*E*total);
	return G;
}

int main (int argc, char **argv) {

	print_time();

	int L, seed, D_12, limitN;
	double theta, lambda;
	tdpotn_argcv(argc, argv, &L, &seed, &D_12, &limitN, &theta, &lambda);

	set_seed_MTPR(seed);

	int kk;
	for (kk = 0; kk < 41; ++kk) {
		double alpha = kk * 0.1;

		struct LineFile *baself = tdpotn_lf(L, D_12);
		struct iiNet *base = create_iiNet(baself);
		struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
		free(airlf->d1);
		airlf->d1 = NULL;

		struct LineFile *alllf = add_LineFile(baself, airlf);
		double *xe = malloc(alllf->linesNum * sizeof(double));
		assert(xe != NULL);
		int i;
		for (i = 0; i < alllf->linesNum; ++i) {
			xe[i] = 0.0;
		}
		alllf->d1 = xe;
		struct iidNet *all = create_iidNet(alllf);
		free_LineFile(alllf);
		free_LineFile(airlf);
		free_LineFile(baself);

		double avesp;
		get_XE_iiNet(base, all, &avesp);
		double gini = calculate_gini(all, base);
		printf("result: \t%f\t%f\t%f\n", alpha, avesp, gini);
		free_iiNet(base);
		free_iidNet(all);
	}

	print_time();
	return 0;
}
