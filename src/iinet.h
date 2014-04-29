#ifndef CN_IINET_H
#define CN_IINET_H

#include "linefile.h"

struct iiNet{
	int maxId;
	int minId;
	int idNum;
	long edgesNum;
	long countMax;
	long countMin;
	long *count;
	int **edges;
};

void free_iiNet(struct iiNet *net);
struct iiNet *create_iiNet(const struct LineFile * const file);

void print_iiNet(struct iiNet *net, char *filename);

//thread routine, don't need arg, the routine will use net;
//of course, you can use this in the main thread.
void *verify_iiNet(void *arg);

int *shortestpath_1A_iiNet(struct iiNet *net, int id);
int *get_ALLSP_iiNet(struct iiNet *net);

void get_useRate_iiNet(struct iiNet *net, struct iiNet *air, double *useRate, double *sameRate, double *cleanRate);

int **shortestpath_AA_FW_iiNet(struct iiNet *net);

int *shortestpath_1A_S_iiNet(struct iiNet *net, int id, int step, int *lNum);

void get_coupling_iiNet(struct iiNet *net, struct iiNet *air, double *coupling, double *avesp);

#include "iidnet.h"
void get_XE_iiNet(struct iiNet *net, struct iidNet *XE, double *avesp);

#endif
