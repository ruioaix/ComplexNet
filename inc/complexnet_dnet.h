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
	int **to;
};

//void freeDNet(struct DirectNet *dnet);
void freeDNet(void);
//struct DirectNet *buildDNet(const struct iiLineFile * const file);
void buildDNet(const struct iiLineFile * const file);
struct DirectNet *cloneDNet(const struct DirectNet * const dnet);
int buildIStoDNet(const struct innLine * const is, struct DirectNet *dnet);

struct DirectNet *getDirectNet(void);

struct DNetSpreadArgs {
	struct innLine *IS;
	double infectRate;
	double touchParam;
	int loopNum;
};
//thread routine, args type is struct DNetSpreadArgs;
void *dnet_spread(void * args);
//thread routine, args type is struct DirectNet;
void *verifyDNet(void *arg);

#endif
