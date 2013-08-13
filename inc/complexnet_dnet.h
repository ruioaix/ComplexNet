#ifndef COMPLEXNET_DNET_H
#define COMPLEXNET_DNET_H

#include "../inc/complexnet_file.h"
#include "../inc/complexnet_random.h"

#include <math.h>

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
struct DirectNet *buildDNet(struct NetFile *file);

void buildIStoDNet(struct InfectSource *is, struct DirectNet *dnet);

int dnet_spread_core(struct InfectSource *IS, struct DirectNet *dNet, double infectRate);

int dnet_spread(struct InfectSource *IS, struct DirectNet *dNet, double infectRate, int loopNum);

struct DirectNet *cloneDNet(struct DirectNet *dnet);

#endif
