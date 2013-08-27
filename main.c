//#define NDEBUG
#include <stdio.h>
#include <time.h>
#include "inc/complexnet_file.h" //for readFileLBL;
#include "inc/complexnet_dnet.h" //for buildDNet;
#include "inc/complexnet_random.h"
#include "inc/complexnet_hashtable.h" //for buildDNet;
#include "inc/complexnet_threadpool.h"
#include "inc/complexnet_error.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);

	//argv,argc process;
	char *datafilename="data/data1";
	char *originfilename="data/origin1";
	if (argc==2) datafilename=argv[1];
	else if (argc==3) {
		datafilename=argv[1];
		originfilename=argv[2];
	}
	else if (argc>3) {
		isError("two much args.\n");
	}
	
	//create thread pool.
	int threadMax = 10;
	createThreadPool(threadMax);

	//read data file, create net;
	struct iiLineFile *file=create_iiLineFile(datafilename);
	addWorktoThreadPool(writeContinuousNetFileHT, file);

	buildDNet(file);
	addWorktoThreadPool(verifyDNet, NULL);

	//read origin file, create IS;
	struct innLineFile *ISs=create_innLineFile(originfilename);

	//ISs, NET, infectRate, touchparam, loopNum, Thread_max.
	//dnet_spread(ISs, dnet, 0.9, 0, 30, 1);
	int isNum=ISs->linesNum;
	init_MersenneTwister();
	struct DNetSpreadArgs args_thread[isNum];
	int i;
	for(i=0; i<isNum; ++i) {
		args_thread[i].IS= ISs->lines+i;
		args_thread[i].infectRate = 0.1;
		args_thread[i].touchParam = 0;
		args_thread[i].loopNum = 30;
		addWorktoThreadPool(dnet_spread, args_thread+i);
	}

	destroyThreadPool();

	free_iiLineFile(file);
	free_innLineFile(ISs);
	freeDNet();
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
