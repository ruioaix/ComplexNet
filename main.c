//#define NDEBUG  //for assert
#include "inc/complexnet_file.h" //for readFileLBL;
#include "inc/complexnet_dnet.h" //for buildDNet;
#include "inc/complexnet_dtnet.h" //for buildDNet;
#include "inc/complexnet_random.h"
#include "inc/complexnet_hashtable.h" //for buildDNet;
#include "inc/complexnet_threadpool.h"
#include "inc/complexnet_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);

	char *pEnd;
	int timeScope;
	if (argc == 2) {
		timeScope = strtol(argv[1], &pEnd, 10);
	}
	else {
		timeScope =1;
	}


	struct i4LineFile *file=create_i4LineFile("data/radoslawemail-clean.txt");
	init_DirectTemporalNet(file);
	int maxId=getMaxId_DirectTemporalNet();
	setTimeScope_DirectTemporalNet(timeScope);

	int timeMax=gettimeMax_DirectTemporalNet();
	int timeMin=gettimeMin_DirectTemporalNet();
	printf("%d\n", timeMin);fflush(stdout);
	printf("%d\n", timeMax);fflush(stdout);
	int timeRange = timeMax-timeMin+1;
	printf("%d\n", timeRange);fflush(stdout);

	int *timeStatistics = calloc(timeRange, sizeof(int));
	assert(timeStatistics);

	//create thread pool.
	int threadMax = 10;
	createThreadPool(threadMax);

	char filename[30];
	sprintf(filename, "RESULT/rados_%d", timeScope);
	FILE *fp = fopen(filename, "w");
	fileError(fp, "xx");
	struct DTNetShortPath1NArgs *args[maxId+1];

	pthread_mutex_t mutex_timeStatistics;
	pthread_mutex_init(&mutex_timeStatistics, NULL);
	int i;
	for (i=0; i<maxId+1; ++i) {
		args[i]=malloc(sizeof(struct DTNetShortPath1NArgs));
		args[i]->vtId = i;
		args[i]->fp = fp;
		args[i]->timeStatistics = timeStatistics;
		args[i]->mutex = &mutex_timeStatistics;
		addWorktoThreadPool(shortpath_1n_DTNet, args[i]);
	}
	
	//destroy thread pool.
	destroyThreadPool();
	
	//
	sprintf(filename, "RESULT/rados_timeStatistics_%d", timeScope);
	FILE *fp1 = fopen(filename, "w");
	for (i=0; i<timeRange; ++i) {
		if (timeStatistics[i] != 0) {
			fprintf(fp1, "%d\t%d\n", i+timeMin, timeStatistics[i]);
		}
	}
	fclose(fp1);

	for (i=0; i<maxId+1; ++i) {
		free(args[i]);
	}
	free(timeStatistics);
	free_i4LineFile(file);
	free_DirectTemporalNet();
	fclose(fp);

	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
