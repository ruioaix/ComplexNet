#ifndef COMPLEXNET_DTNET_H
#define COMPLEXNET_DTNET_H

#include "../inc/complexnet_file.h"
#include "stdio.h"
#include "pthread.h"

enum timeScopeDTNet {
	second01 = 1,
	hour01 = 3600  ,
	hour02 = 7200  ,
	hour03 = 10800 ,
	hour04 = 14400 ,
	hour05 = 18000 ,
	hour06 = 21600 ,
	hour07 = 25200 ,
	hour08 = 28800 ,
	hour09 = 32400 ,
	hour10 = 36000 ,
	hour11 = 39600 ,
	hour12 = 43200 ,
	hour13 = 46800 ,
	hour14 = 50400 ,
	hour15 = 54000 ,
	hour16 = 57600 ,
	hour17 = 61200 ,
	hour18 = 64800 ,
	hour19 = 68400 ,
	hour20 = 72000 ,
	hour21 = 75600 ,
	hour22 = 79200 ,
	hour23 = 82800 ,
	hour24 = 86400 ,
	day01 =  86400 ,
	day02 =  172800,
	day03 =  259200,
	day04 =  345600,
	day05 =  432000,
	day06 =  518400,
	day07 =  604800,
};

struct DirectTemporalNet {
	int maxId;
	int minId;
	int vtsNum;
	long edgesNum;
	long inCountMax;
	long outCountMax;
	int timeScope;
	long *outCount;
	long *inCount;
	int **out;
	int **outTemporal;	
	int **outTemporal_second01;	
	int timeMax;
	int timeMin;
	int timeMax_second01;
	int timeMin_second01;
};

struct DTNetShortPath1NArgs {
	int vtId;
	FILE *fp;
	int *timeStatistics;
	pthread_mutex_t *mutex;
};

void init_DirectTemporalNet(const struct i4LineFile* const file);
void free_DirectTemporalNet(void);
void setTimeScope_DirectTemporalNet(int timeScope);
void *verifyDTNet(void *arg);

void *shortpath_1n_DTNet(void *arg);

int getMaxId_DirectTemporalNet();
int gettimeScope_DirectTemporalNet();
int gettimeMax_DirectTemporalNet();
int gettimeMin_DirectTemporalNet();

#endif
