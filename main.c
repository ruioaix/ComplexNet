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

	//init MT random number generator
	//unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
	//init_by_array_MersenneTwister(init, length);

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
	

	//read data file, create net;
	struct iiLineFile *file=create_iiLineFile(datafilename);
	struct DirectNet *dnet=buildDNet(file);

	//pthread_t tid;
	//pthread_create(&tid, NULL, verifyDNet, dnet);
	//pthread_create(&tid, NULL, writeContinuousNetFileHT, file);

	//read origin file, create IS;
	struct innLineFile *ISs=create_innLineFile(originfilename);


	//ISs, NET, infectRate, touchparam, loopNum, Thread_max.
	//dnet_spread(ISs, dnet, 0.9, 0, 30, 1);
	double infectRate = 0.1;
	double touchParam= 0;
	int loopNum = 30;
	int threadMax = 10;

	init_MersenneTwister();

	createThreadPool(threadMax);

	int isNum=ISs->linesNum;
	struct DNetSpreadArgs args_thread[isNum];
	int i;
	addWorktoThreadPool(writeContinuousNetFileHT, file);
	addWorktoThreadPool(verifyDNet, dnet);
	for(i=0; i<isNum; ++i) {
		args_thread[i].dNet = dnet;
		args_thread[i].IS= ISs->lines+i;
		args_thread[i].infectRate = infectRate;
		args_thread[i].touchParam = touchParam;
		args_thread[i].loopNum = loopNum;
		addWorktoThreadPool(dnet_spread, args_thread+i);
	}

	destroyThreadPool();



	free_iiLineFile(file);
	free_innLineFile(ISs);
	//pthread_join(tid, NULL);
	freeDNet(dnet);
	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	//pthread_exit(NULL);
	return 0;
}
