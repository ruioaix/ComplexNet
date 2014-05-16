#ifndef CN_ROBUST_H
#define CN_ROBUST_H

#include "linefile.h"
#include "iinet.h"

struct LineFile *robust_ER_or_SF(int es, int N, int seed, int MM0);
void robust_argc_argv(int argc, char **argv, int *es, int *N, int *seed, int *MM0, int *kor, double *q, int *algorithm_type, int *pairsNum);
int *robust_create_deletelist(struct iiNet *net, int kor);

struct CoupLink {
	struct LineFile *lid_i12;
	struct i3Net *i12_lid;
	int *lid_gid;
	int gidMax;
	int gidCountMax;
	int gidCountMin;
	int *gidCount;
	int **gid_lids;
};
struct CoupLink * robust_get_cplk(struct LineFile *lid_i12, double q, int algorithm_type, int pairsNum);
void free_CPLK(struct CoupLink *cplk);

#endif
