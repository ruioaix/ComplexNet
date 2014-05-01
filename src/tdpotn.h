#ifndef CN_TDPOTN_H
#define CN_TDPOTN_H

#include "linefile.h"
#include "iinet.h"

void tdpotn_argcv(int argc, char **argv, int *L, int *seed, int *D_12, int *limitN, double *theta);
struct LineFile * tdpotn_lf(int L, int D_12);
struct LineFile *tdpotn_air_all(struct iiNet * net, double alpha, int limitN, double theta);

#endif
