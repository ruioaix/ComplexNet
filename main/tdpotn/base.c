#include <stdio.h>
#include <stdlib.h>
#include "base.h"
#include "iinet.h"
#include "spath.h"
#include "mtprand.h"
#include "tdpotn.h"

/**
 * default: ./tdpotn-base 50 1 1 5 1.0 0
 * N = 50, for 1d ring, it's the idNum. for 2d lattice, it's the height&width of lattice; for example, if N = 50, the nodes of lattice is 2500.
 * seed = 1, it's the seed for PRNG.
 * D_12 = 1, 1 means 1d, 2 means 2d.
 * limitN = 5, the constrain for this research, C = limitN * nodeNum.
 * theta = 1.0, it will change air. to geit will change air. to guarantee we can generate air successful, theta >= 0.5.
 * 	theta decide the cost of building a new edges in air, if theta < 1, the cost of long edges is not so much, 
 * 	else the cost of long edges is much more expensive than short.
 * lambda = 1, this decide the effect of a air edge. 
 * 	if lambda = 0, all air edges's weight is 1, it's the best experience.
 * 	if lambda = 1, all air edges has no advance to base net. it's the worst experience.
 * 	lambda affect the the value of airlf->d1, and in the program, airlf->d1 is not used. so it's not important for base.c.
 */
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
