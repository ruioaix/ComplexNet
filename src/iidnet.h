#ifndef CN_IIDNET_H
#define CN_IIDNET_H

#include "iidlinefile.h"

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
void sort_desc_iidNet(struct iidNet *net);
void sort_asc_iidNet(struct iidNet *net);

#endif
