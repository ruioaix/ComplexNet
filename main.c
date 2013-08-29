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
	
	//struct i4LineFile *file=create_i4LineFile("data/eronClean2.txt");
	//init_DirectTimeNet(file);
	struct iiLineFile *file=create_iiLineFile("data/digg_friendd.txt");
	struct innLineFile *is=create_innLineFile("data/digg_friendsDRTopAllTop1000Overlap.txt");
	init_MersenneTwister();
	buildDNet(file);
	int i;
	struct DNetSpreadArgs args[is->linesNum];
	for (i=0; i<is->linesNum; ++i) {
		args[i].IS=is->lines+i;	
		args[i].infectRate=0.1;
		args[i].touchParam=0;
		args[i].loopNum=10;
		addWorktoThreadPool(dnet_spread, args+i);
	}

	//destroy thread pool.
	destroyThreadPool();
	free_iiLineFile(file);
	free_innLineFile(is);
	freeDNet();

	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
