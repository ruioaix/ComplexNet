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

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);

	struct i4LineFile *file=create_i4LineFile("data/radoslawemail-clean.txt");
	init_DirectTemporalNet(file);
	int maxId=getMaxId_DirectTemporalNet();

	//create thread pool.
	int threadMax = 10;
	createThreadPool(threadMax);

	char filename[30];
	int timeScope = gettimeScope_DirectTemporalNet();
	sprintf(filename, "RESULT/redos_%d", timeScope);
	FILE *fp = fopen(filename, "w");
	fileError(fp, "xx");
	struct DTNetShortPath1NArgs *args[maxId+1];
	int i;
	for (i=0; i<maxId+1; ++i) {
		args[i]=malloc(sizeof(struct DTNetShortPath1NArgs));
		args[i]->vtId = i;
		args[i]->fp = fp;
		addWorktoThreadPool(shortpath_1n_DTNet, args[i]);
	}
	
	//destroy thread pool.
	destroyThreadPool();
	for (i=0; i<maxId+1; ++i) {
		free(args[i]);
	}
	free_i4LineFile(file);
	free_DirectTemporalNet();
	fclose(fp);

	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
