#ifndef CN_IIDNET_H
#define CN_IIDNET_H

#include "linefile.h"

struct iidNet{
	int maxId;
	int minId;
	int idNum;
	long edgesNum;
	long countMax;
	long countMin;
	long *count;
	int **edges;
	double **d;
};

void free_iidNet(struct iidNet *net);
struct iidNet *create_iidNet(const struct LineFile * const file);

void print_iidNet(struct iidNet *net, char *filename);
void sort_desc_iidNet(struct iidNet *net);
void sort_asc_iidNet(struct iidNet *net);

#endif
