#ifndef COMPLEXNET_DTNET_H
#define COMPLEXNET_DTNET_H

#include "../inc/complexnet_file.h"

struct DirectTimeNet {
	int maxId;
	int minId;
	int vtsNum;
	long edgesNum;
	long inCountMax;
	long outCountMax;
	int timeMax;
	int timeMin;
	int *inTimeDoor;
	long *outCount;
	long *inCount;
	int **out;
	int **outTime;	
};


void init_DirectTimeNet(const struct i4LineFile* const file);
void *verifyDTNet(void *arg);

#endif
