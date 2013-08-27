#ifndef COMPLEXNET_DNET_H
#define COMPLEXNET_DNET_H

#include "../inc/complexnet_file.h"

struct DirectNet{
	int maxId;
	int minId;
	int vtsNum;
	long edgesNum;
	long countMax;
	long *count;
	char *status;
	int **to;
};

enum touchtype {all=0, part=1};

void freeDNet(struct DirectNet *dnet);
struct DirectNet *buildDNet(const struct iiLineFile * const file);

int buildIStoDNet(const struct innLine * const is, struct DirectNet *dnet);

struct DNetSpreadCoreArgs {
	struct innLine *IS;
	struct DirectNet *dNet;
	double infectRate;
	double touchParam;
	int loopNum;
	int isId;
};

void *dnet_spread(void * args);

struct DirectNet *cloneDNet(const struct DirectNet * const dnet);
void *verifyDNet(void *arg);

#endif
