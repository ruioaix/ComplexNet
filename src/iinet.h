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

void verify_connectedness_iiNet(struct iiNet *net);

int robust_iiNet(struct iiNet *net);

void delete_node_iiNet(struct iiNet *net, int id);


#include "iidnet.h"
void get_XE_iiNet(struct iiNet *net, struct iidNet *XE, double *avesp);

#endif
