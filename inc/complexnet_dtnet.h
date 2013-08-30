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
	int *inDoorMax;
	int *inDoorMin;
	long *outCount;
	long *inCount;
	int **out;
	int **outTemporal;	
};


void init_DirectTemporalNet(const struct i4LineFile* const file);
void free_DirectTemporalNet(void);
void *verifyDTNet(void *arg);

int shortpath_11_DTNet(int id_from, int id_to);

#endif
