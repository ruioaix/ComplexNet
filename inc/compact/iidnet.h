#ifndef IIDNET_H
#define IIDNET_H

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

#endif
