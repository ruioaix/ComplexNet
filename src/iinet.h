#ifndef CN_IINET_H
#define CN_IINET_H

#include "linefile.h"

struct iiNet{
	int maxId;
	int minId;
	int idNum;
	long edgesNum;
	long countMax;
	long countMin;
	long *count;
	int **edges;
};

void free_iiNet(struct iiNet *net);
struct iiNet *create_iiNet(const struct LineFile * const file);
void print_iiNet(struct iiNet *net, char *filename);

long *degree_distribution_iiNet(struct iiNet *net);
void delete_node_iiNet(struct iiNet *net, int id);

void verify_duplicatePairs_iiNet(struct iiNet *net);
void verify_fullyConnected_iiNet(struct iiNet *net);

int robust_iiNet(struct iiNet *net);


#endif
