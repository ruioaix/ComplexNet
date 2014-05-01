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
		struct iidNet *air = create_iidNet(airlf);
		free_LineFile(airlf);

		double avesp;
		avesp_spath03_Net(base, air, &avesp);
		printf("result: lamba: %f, alpha: %f, avesp: %f\n", lambda, alpha, avesp);

		free_iiNet(base);
		free_iidNet(air);

	}

	print_time();
	return 0;
}
