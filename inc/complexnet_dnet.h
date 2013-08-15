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
struct DirectNet *buildDNet(const struct NetFile * const file);

void buildIStoDNet(const struct InfectSource * const is, struct DirectNet *dnet);

int dnet_spread_core(const struct InfectSource * const IS, struct DirectNet *dNet, const double infectRate, const double touchParam);

int dnet_spread(const struct InfectSourceFile * const IS, const struct DirectNet * const dNet, const double infectRate, const double touchParam, const int loopNum);

struct DirectNet *cloneDNet(const struct DirectNet * const dnet);

#endif
