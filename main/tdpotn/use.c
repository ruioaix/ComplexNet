#include <stdio.h>
#include "base.h"
#include "iinet.h"
#include "spath.h"
#include "mtprand.h"
#include "tdpotn.h"

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
		free_LineFile(baself);
		struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
		struct iiNet *air = create_iiNet(airlf);
		free_LineFile(airlf);

		double useRate, sameRate, cleanRate, avesp;
		useRate_spath04_iiNet(base, air, &useRate, &sameRate, &cleanRate, &avesp);
		free_iiNet(base);
		free_iiNet(air);
		printf("result:\t%d\t%f\t%f\t%f\t%f\t%f\t%f\n", limitN, alpha, useRate, sameRate, cleanRate, useRate+sameRate+cleanRate, avesp);

	}

	print_time();
	return 0;
}