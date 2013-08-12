#ifndef COMPLEXNET_DNET_H
#define COMPLEXNET_DNET_H

#include "../inc/complexnet_file.h"
#include "../inc/complexnet_random.h"

#include <math.h>

struct DirectNet{
	idtype maxId;
	idtype minId;
	linesnumtype edgesNum;
	linesnumtype countMax;
	linesnumtype *count;
	char *status;
	idtype **to;
};

enum touchtype {all=0, part=1};

void freeDNet(struct DirectNet *dnet);
struct DirectNet *buildDNet(struct NetFile *file);

void buildIStoDNet(struct InfectSource *is, struct DirectNet *dnet);

int dnet_spread_touch_all_core(struct InfectSource *IS, struct DirectNet *dNet, double infectRate);
int dnet_spread_touch_part_core(struct InfectSource *IS, struct DirectNet *dNet, double infectRate);

int dnet_spread(struct InfectSource *IS, struct DirectNet *dNet, double infectRate, int loopNum, enum touchtype tt);

struct DirectNet *createDNetFormDNet(struct DirectNet *dnet);
void cloneDNet(struct DirectNet *dnet_c, struct DirectNet *dnet);

#endif
