#ifndef CN_ROBUST_H
#define CN_ROBUST_H

#include "linefile.h"
#include "iinet.h"

struct LineFile *robust_ER_or_SF(int es, int N, int seed, int MM0);
void robust_argc_argv(int argc, char **argv, int *es, int *N, int *seed, int *MM0, int *kor, double *q, int *coupNum);
int *robust_deletelist(struct iiNet *net, int kor);

#endif
