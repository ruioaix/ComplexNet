#ifndef COMPLEXNET_DTNET_H
#define COMPLEXNET_DTNET_H

#include "../inc/complexnet_file.h"

struct DirectTemporalNet {
	int maxId;
	int minId;
	int vtsNum;
	long edgesNum;
	long inCountMax;
	long outCountMax;
	int timeMax;
	int timeMin;
	long *outCount;
	long *inCount;
	int **out;
	int **outTemporal;	
};


void init_DirectTemporalNet(const struct i4LineFile* const file);
void free_DirectTemporalNet(void);
void *verifyDTNet(void *arg);

void *shortpath_1n_DTNet(void *arg);

int getMaxId_DirectTemporalNet();

#endif
