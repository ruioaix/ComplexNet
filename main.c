//#define NDEBUG  //for assert
#include "inc/complexnet_file.h" //for readFileLBL;
#include "inc/complexnet_dnet.h" //for buildDNet;
#include "inc/complexnet_dtnet.h" //for buildDNet;
#include "inc/complexnet_random.h"
#include "inc/complexnet_hashtable.h" //for buildDNet;
#include "inc/complexnet_threadpool.h"
#include "inc/complexnet_error.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);

	//create thread pool.
	int threadMax = 10;
	createThreadPool(threadMax);

	if (argc != 3) isError("not right args");
	char *pEnd;
	int begin=strtol(argv[1], &pEnd, 10);
	int end=strtol(argv[2], &pEnd, 10);
	
	//struct i4LineFile *file=create_i4LineFile("data/testdtnet");
	//struct i4LineFile *file=create_i4LineFile("data/eronClean2.txt");
	struct i4LineFile *file=create_i4LineFile("data/test01.txt");
	init_DirectTemporalNet(file);

	//addWorktoThreadPool(verifyDTNet, NULL);
	//printf("%d\n", shortpath_11_DTNet(3, 2));
	int i;
	int maxId=getMaxId_DirectTemporalNet();
	end = maxId>end?end:maxId;
	//addWorktoThreadPool(shortpath_1n_DTNet, (void *)0);
	for (i=begin; i<=end; ++i) {
		addWorktoThreadPool(shortpath_1n_DTNet, (void *)i);
	}
	
	//destroy thread pool.
	destroyThreadPool();
	free_i4LineFile(file);
	free_DirectTemporalNet();

	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
