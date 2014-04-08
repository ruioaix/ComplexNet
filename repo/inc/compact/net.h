#ifndef COMPLEXNET_NET_H
#define COMPLEXNET_NET_H

#include "../linefile/iilinefile.h"

struct Net{
	int maxId;
	int minId;
	int vtsNum;
	long edgesNum;
	long countMax;
	long *count;
	int **edges;
};
void free_Net(struct Net *net);
struct Net *create_Net(const struct iiLineFile * const file);
void print_Net(struct Net *net, char *filename);

//thread routine, don't need arg, the routine will use net;
//of course, you can use this in the main thread.
void *verifyNet(void *arg);

//DMP algorithm
//void net_dmp(int T);
void net_dmp(struct Net *net, int T, double infect_rate, double recover_rate);
double *net_dmp_is(struct Net *net, int infect_source, int T, double infect_rate, double recover_rate);
//void net_dmp(int T, double infect_rate, double recover_rate);
long net_find_index(struct Net *net, int v1, int v2);

#endif
