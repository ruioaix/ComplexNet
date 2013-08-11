#ifndef COMPLEXNET_DNET_H
#define COMPLEXNET_DNET_H

#include "../inc/complexnet_file.h"
#include "../inc/complexnet_random.h"

struct DirectNet{
	idtype maxId;
	idtype minId;
	linesnumtype edgesNum;
	linesnumtype countMax;
	linesnumtype *count;
	char *status;
	idtype **to;
};

struct InfectSource {
	idtype *vt;
	idtype num;
};

struct DirectNet *buildDNet(struct NetFile *file);
int spread_touch_all(struct InfectSource *IS, struct DirectNet *dNet, double infectRate);

#endif
