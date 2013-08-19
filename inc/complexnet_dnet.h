#ifndef COMPLEXNET_DNET_H
#define COMPLEXNET_DNET_H

#include "../inc/complexnet_file.h"
#include "../inc/complexnet_random.h"

#include <math.h>
#include <pthread.h>


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
};

void *dnet_spread_core(void * args);
//int dnet_spread_core(const struct InfectSource * const IS, const struct DirectNet * const dNet, const double infectRate, const double touchParam, const int loopNum);

//int dnet_spread(struct InfectSourceFile * IS, struct DirectNet * dNet, double infectRate, double touchParam, int loopNum);
int dnet_spread(struct InfectSourceFile * IS, struct DirectNet * dNet, double infectRate, double touchParam, int loopNum, int threadMax);
//int dnet_spread(const struct InfectSourceFile * const IS, const struct DirectNet * const dNet, const double infectRate, const double touchParam, const int loopNum);

struct DirectNet *cloneDNet(const struct DirectNet * const dnet);

#endif
