#ifndef COMPLEXNET_DNET_H
#define COMPLEXNET_DNET_H

#include "../inc/complexnet_file.h"

struct DirectNet *buildDNet(struct NetFile *file);

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

#endif
