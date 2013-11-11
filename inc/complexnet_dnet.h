#ifndef COMPLEXNET_DNET_H
#define COMPLEXNET_DNET_H

#include "../inc/linefile/iilinefile.h"
#include "../inc/linefile/innlinefile.h"
#include "stdio.h"

struct DirectNet{
	int maxId;
	int minId;
	int vtsNum;
	long edgesNum;
	long countMax;
	long *count;
	int **to;
};
void freeDNet(void);
void buildDNet(const struct iiLineFile * const file);
struct DirectNet *cloneDNet(const struct DirectNet * const dnet);
struct DirectNet *getDirectNet(void);

struct DNetSpreadArgs {
	struct innLine *IS;
	double infectRate;
	double touchParam;
	int loopNum;
	FILE *fp;
};
//thread routine, args type is struct DNetSpreadArgs;
void *dnet_spread(void * arg);
//thread routine, don't need arg, the routine will use dnet;
void *verifyDNet(void *arg);

#endif
