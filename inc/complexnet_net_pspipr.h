#ifndef COMPLEXNET_NET_PSPIPR_H
#define COMPLEXNET_NET_PSPIPR_H


#include "../inc/complexnet_file.h"

struct Net_PSPIPR{
	int maxId;
	int minId;
	int vtsNum;
	long edgesNum;
	long countMax;
	long *count;
	int **edges;
	double **PS;
	double **PI;
	double **PR;
};
void free_Net_PSPIPR(void);
void create_Net_PSPIPR(const struct iid3LineFile * const file);

#endif
