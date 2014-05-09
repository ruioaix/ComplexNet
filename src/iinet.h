#ifndef CN_IINET_H
#define CN_IINET_H

#include "linefile.h"

struct iiNet{
	//these four will be right when net is initially created.
	//but when nodes or links are added or deleted, it's non-efficient to keep 
	//the following four right. especially when added or deleted frequently.
	//so sometimes, they are wrong.
	int maxId;
	int minId;
	long countMax;
	long countMin;

	//these four is always right.
	int idNum;
	long edgesNum;
	long *count;
	int **edges;
};

void free_iiNet(struct iiNet *net);
struct iiNet *create_iiNet(const struct LineFile * const file);
void print_iiNet(struct iiNet *net, char *filename);

long *degree_distribution_iiNet(struct iiNet *net);
int delete_node_iiNet(struct iiNet *net, int id);
int delete_link_iiNet(struct iiNet *net, int id, int neigh);

void verify_duplicatePairs_iiNet(struct iiNet *net);
void verify_fullyConnected_iiNet(struct iiNet *net);

int robust_iiNet(struct iiNet *net, int *robust);


#endif
