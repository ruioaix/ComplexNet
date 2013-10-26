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
//of course, you can use this in the main thread.
void *verifyNet(void *arg);

//DMP algorithm
void net_dmp(void);
long net_find_index(int v1, int v2);

#endif
