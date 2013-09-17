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
	if (argc == 3) {
		timeScope = strtol(argv[2], &pEnd, 10);
	}
	else {
		isError("wrong usage");
	}

	struct i4LineFile *file=create_i4LineFile(argv[1]);
	init_DirectTemporalNet(file);
	int maxId=getMaxId_DirectTemporalNet();
	int timeMax_second01 = gettimeMax_DirectTemporalNet();
	int timeMin_second01 = gettimeMin_DirectTemporalNet();
	int timeRange = timeMax_second01-timeMin_second01+1;
	//printf("%d\n", timeRange);fflush(stdout);

	setTimeScope_DirectTemporalNet(timeScope);

	struct HashTable *ht = createHashTable(1000000);
	int i;
	for (i=0; i<file->linesNum; ++i) {
		insertHEtoHT(ht, file->lines[i].i4/timeScope);
	}
	int timeMax_hash = getelementSumNumHT(ht);
	printf("Number of different time moments: %d\n", timeMax_hash);

while(1);

	int *timeStatistics = calloc(timeMax_hash+1, sizeof(int));
	assert(timeStatistics != NULL);

	//create thread pool.
	int threadMax = 10;
	createThreadPool(threadMax);

	struct DTNetShortPath1NArgs **args=malloc((maxId+1)*sizeof(struct DTNetShortPath1NArgs));
	assert(args != NULL);

	pthread_mutex_t mutex_timeStatistics;
	pthread_mutex_init(&mutex_timeStatistics, NULL);
	printf("xxxx\n"); fflush(stdout);
	for (i=0; i<maxId+1; ++i) {
		args[i]=malloc(sizeof(struct DTNetShortPath1NArgs));
		assert(args[i] != NULL);
		args[i]->vtId = i;
		args[i]->ht = ht;
		args[i]->timeStatistics = timeStatistics;
		args[i]->mutex = &mutex_timeStatistics;
		addWorktoThreadPool(shortpath_1n_DTNet, args[i]);
	}
	
	//destroy thread pool.
	destroyThreadPool();
	
	char filename[300];
	sprintf(filename, "RESULT/rados_timeStatistics_%s_%d", argv[1], timeScope);
	FILE *fp1 = fopen(filename, "w");
	long sp_sum = 0;
	long ed = 0;
	for (i=0; i<timeRange; ++i) {
		if (timeStatistics[i] != 0) {
			fprintf(fp1, "%ld\t%d\n", getelementValueHT(ht, i), timeStatistics[i]);
		}
		sp_sum += i*timeStatistics[i];
		ed += timeStatistics[i];
	}
	double sp_avg = (double)sp_sum/(double)ed;
	fclose(fp1);

	printf("timeScope: %d, sp_avg : %f\n", timeScope, sp_avg);
	for (i=0; i<maxId+1; ++i) {
		free(args[i]);
	}
	free(timeStatistics);
	free_i4LineFile(file);
	free_DirectTemporalNet();
	//fclose(fp);

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
