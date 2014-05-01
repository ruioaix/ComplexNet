#include <stdio.h>
#include <stdlib.h>
#include "base.h"
#include "iinet.h"
#include "spath.h"
#include "mtprand.h"
#include "tdpotn.h"

//paper: ./tdpotn-base 50 1 1 5 1.0 1
//N = 50, seed = 1, D_12 = 1, limitN = 5, theta = 1.0, lambda = 1(it 's not important for base.c).
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
		free_iiNet(base);
		free(airlf->d1);
		airlf->d1 = NULL;
		struct LineFile *alllf = add_LineFile(baself, airlf);
		free_LineFile(baself);
		free_LineFile(airlf);
		struct iiNet *all = create_iiNet(alllf);
		free_LineFile(alllf);

		double avesp;
	   	avesp_spath01_iiNet(all, &avesp);
		free_iiNet(all);

		printf("result =>>\talpha:\t%f\tavesp:\t%f\n", alpha, avesp);
	}
	print_time();
	return 0;
}
