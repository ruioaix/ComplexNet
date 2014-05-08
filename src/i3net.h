#ifndef CN_I3NET_H
#define CN_I3NET_H

#include "linefile.h"

struct i3Net{
	int maxId;
	int minId;
	int idNum;
	long edgesNum;
	long countMax;
	long countMin;
	long *count;
	int **edges;
	int **i3;
};

void free_i3Net(struct i3Net *net);
struct i3Net *create_i3Net(const struct LineFile * const file);
void print_i3Net(struct i3Net *net, char *filename);

#endif
