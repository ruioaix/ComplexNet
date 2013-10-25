#ifndef COMPLEXNET_NET_H
#define COMPLEXNET_NET_H

#include "../inc/complexnet_file.h"

struct Net{
	int maxId;
	int minId;
	int vtsNum;
	long edgesNum;
	long countMax;
	long *count;
	int **edges;
};
void free_Net(void);
void create_Net(const struct iiLineFile * const file);
struct Net *get_Net(void);

//thread routine, don't need arg, the routine will use net;
void *verifyNet(void *arg);

#endif
