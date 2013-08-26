#ifndef COMPLEXNET_DNET_H
#define COMPLEXNET_DNET_H

#include "../inc/complexnet_file.h"

struct DirectNet{
	vttype maxId;
	vttype minId;
	vttype vtsNum;
	edtype edgesNum;
	edtype countMax;
	edtype *count;
	char *status;
	vttype **to;
};

enum touchtype {all=0, part=1};

void freeDNet(struct DirectNet *dnet);
struct DirectNet *buildDNet(const struct NetFile * const file);

int buildIStoDNet(const struct InfectSource * const is, struct DirectNet *dnet);

struct DNetSpreadCoreArgs {
	struct InfectSource *IS;
	struct DirectNet *dNet;
	double infectRate;
	double touchParam;
	int loopNum;
	vttype isId;
};

void *dnet_spread(void * args);

struct DirectNet *cloneDNet(const struct DirectNet * const dnet);
void *verifyDNet(void *arg);

#endif
