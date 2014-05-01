#include <stdio.h>
#include "base.h"
#include "iinet.h"
#include "spath.h"
#include "mtprand.h"
#include "tdpotn.h"

int main (int argc, char **argv) {
	print_time();

	int L, seed, D_12, limitN;
	tdpotn_argcv(argc, argv, &L, &seed, &D_12, &limitN);

	set_seed_MTPR(seed);

	int kk;
	for (kk = 0; kk < 41; ++kk) {
		double alpha = kk * 0.1;

		struct LineFile *baself = tdpotn_lf(L, D_12);
		struct iiNet *base = create_iiNet(baself);
		struct LineFile *airlf = tdpotn_air_all(base, alpha, limitN);
		free_iiNet(base);
		struct LineFile *alllf = add_LineFile(baself, airlf);
		free_LineFile(baself);
		free_LineFile(airlf);
		struct iiNet *all = create_iiNet(alllf);
		free_LineFile(alllf);

		double avesp = avesp_spath01_iiNet(all);
		free_iiNet(all);

		printf("result =>>\talpha:\t%f\tavesp:\t%f\n", alpha, avesp);
	}
	print_time();
	return 0;
}
