//#define NDEBUG  //for assert
#include "inc/complexnet_linefile.h" //for readFileLBL;
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
	struct DNetSpreadArgs args[10][is->linesNum];
	int j;
	FILE *fp[10];
	for (j=0; j<10; ++j) {
		char filename[50];
		sprintf(filename, "Result/dnet_spread_%.2f.txt", 0.02+0.02*j); 
		fp[j] = fopen(filename,"w");
		fileError(fp[j], filename);
		for (i=0; i<is->linesNum; ++i) {
			args[j][i].IS=is->lines+i;	
			args[j][i].infectRate=0.02+0.02*j;
			args[j][i].touchParam=0;
			args[j][i].loopNum=2;
			args[j][i].fp=fp[j];
			addWorktoThreadPool(dnet_spread, args[j]+i);
		}
	}

	//destroy thread pool.
	destroyThreadPool();
	for (j=0; j<10; ++j) {
		fclose(fp[j]);
	}

	free_iiLineFile(file);
	free_innLineFile(is);
	freeDNet();

	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
