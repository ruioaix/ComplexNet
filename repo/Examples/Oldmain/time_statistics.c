//#define NDEBUG  //for assert
#include "inc/complexnet_linefile.h" //for readFileLBL;
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
	setTimeScope_DirectTemporalNet(timeScope);

	struct HashTable *ht = createHashTable(1000000);
	struct DirectTemporalNet *dtnet = getDTNet();
	int i,j;
	for (i=0; i<dtnet->maxId+1; ++i) {
		for (j=0; j<dtnet->outCount[i]; ++j) {
			insertHEtoHT(ht, dtnet->outTemporal[i][j]);
		}
	}
	int diffMoments = getelementSumNumHT(ht);
	printf("The Number of different time moments: %d\n", diffMoments);
	fflush(stdout);
	setelementIndexHT(ht);

	//for (i=0; i<diffMoments; ++i) {
	//	long element = getelementValueHT(ht, i);
	//	int x = getelementIndexHT(ht, element);
	//	if (x != i) {
	//		printf("%d, %ld, %d\n", x, element, i);
	//		fflush(stdout);
	//	}
	//}
	//printf("hahaha\n");
	//return 0;

	int *timeStatistics = calloc(diffMoments, sizeof(int));
	assert(timeStatistics != NULL);

	//create thread pool.
	int threadMax = 10;
	createThreadPool(threadMax);

	struct DTNetShortPath1NArgs **args=malloc((maxId+1)*sizeof(struct DTNetShortPath1NArgs));
	assert(args != NULL);

	pthread_mutex_t mutex_timeStatistics;
	pthread_mutex_init(&mutex_timeStatistics, NULL);
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
	sprintf(filename, "RESULT/rados_timeStatistics_%c_%d", argv[1][7], timeScope);
	FILE *fp1 = fopen(filename, "w");
	long sp_sum = 0;
	long ed = 0;
	long element;
	for (i=0; i<diffMoments; ++i) {
		if (timeStatistics[i] != 0) {
			element = getelementValueHT(ht, i)*timeScope;
			fprintf(fp1, "%ld\t%d\n", element, timeStatistics[i]);
			sp_sum += element*timeStatistics[i];
			ed += timeStatistics[i];
		}
	}
	double sp_avg = (double)sp_sum/(double)ed;
	fclose(fp1);
	printf("timeScope: %d, sp_avg : %f\n", timeScope, sp_avg);

	for (i=0; i<maxId+1; ++i) {
		free(args[i]);
	}
	free(args);
	free(timeStatistics);
	free_i4LineFile(file);
	free_DirectTemporalNet();
	freeHashTable(ht);

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
