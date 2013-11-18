#ifndef COMPLEXNET_NET_H
#define COMPLEXNET_NET_H

#include "../linefile/iidlinefile.h"

struct iidNet{
	int maxId;
	int minId;
	int vtsNum;
	long edgesNum;
	long countMax;
	long *count;
	int **edges;
	double **d3;
};
void free_iidNet(struct iidNet *net);
struct iidNet *create_iidNet(const struct iidLineFile * const file);
void print_iidNet(struct iidNet *net, char *filename);

//thread routine, don't need arg, the routine will use net;
//of course, you can use this in the main thread.
//void *verifyiidNet(void *arg);

#endif
