//#define NDEBUG
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "inc/complexnet_file.h" //for readFileLBL;
#include "inc/complexnet_dnet.h" //for buildDNet;
#include "inc/complexnet_dtnet.h" //for buildDNet;
#include "inc/complexnet_random.h"
#include "inc/complexnet_hashtable.h" //for buildDNet;
#include "inc/complexnet_threadpool.h"
#include "inc/complexnet_error.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);

	//argv,argc process;
	//char *datafilename="data/data1";
	//char *originfilename="data/origin1";
	//if (argc==2) datafilename=argv[1];
	//else if (argc==3) {
	//	datafilename=argv[1];
	//	originfilename=argv[2];
	//}
	//else if (argc>3) {
	//	isError("two much args.\n");
	//}
	
	//create thread pool.
	int threadMax = 10;
	createThreadPool(threadMax);
	
	FILE *fp = fopen("Result/top10_100_refer.dat", "w");
	fileError(fp, "xxx");
	static int l=0;

	struct i5sdLineFile *file=create_i5sdLineFile("data/refer_uniq.dat");
	int i;
	int * status = calloc(file->iMax+1,sizeof(int));
	//for (i=0; i<file->linesNum; ++i) {
	//	int id = file->lines[i].i1;
	//	if (!status[id]) {
	//		fprintf(fp, "%d, %d, %d, %d, %d, %s, %f\n", file->lines[i].i1, file->lines[i].i2,file->lines[i].i3,file->lines[i].i4,file->lines[i].i5,file->lines[i].s6,file->lines[i].d7);
	//		status[id]=1;
	//	}
	//}
	struct iidiLineFile *file_top = create_iidiLineFile("data/top10_100");
	int j;
	char st=0;
	int iii=0;
	for (i=0; i<file_top->linesNum; ++i) {
		int id = file_top->lines[i].i2;
		st = 0;
		for (j=0; j<file->linesNum; ++j) {
			int idd = file->lines[j].i1;
			if (id == idd) {
				fprintf(fp, "%d,\t%d,\t%f,\t%d,\t%d/%d/%d,\t%d,\t%f\n", file_top->lines[i].i1, file_top->lines[i].i2,file_top->lines[i].d3,file_top->lines[i].i4,file->lines[j].i3,file->lines[j].i4,file->lines[j].i5,file->lines[j].i2,file->lines[j].d7);
				st  = 1;
			}
		}
		if (st == 0) {
			++iii;
			fprintf(fp, "%d,\t%d,\t%f,\t%d,\n", file_top->lines[i].i1, file_top->lines[i].i2,file_top->lines[i].d3,file_top->lines[i].i4);
		}
	}
	printf("%d\n", iii);

/*
	int ii=0;
	for (ii=0; ii<101; ++ii) {
		char filename[100];
		sprintf(filename, "data/items/items_0_0.0_1.0_0.0_1.0_0.0_0.0_%d.dat", ii);
		struct idiLineFile *file=create_idiLineFile(filename);
		double sort[file->linesNum];
		int sort_index[file->linesNum];
		long i;
		for (i=0; i<file->linesNum; ++i) {
			sort[i] = file->lines[i].d2;
			sort_index[i]= file->lines[i].i1;
		}
		quick_sort_double_index(sort, 0, file->linesNum-1, sort_index);
		//for (i=0; i<file->linesNum; ++i) {
		//	printf("%f,%d\n", sort[i], sort_index[i]);
		//}
		for (i=1; i<11; ++i) {
			int id=sort_index[file->linesNum-i];
			fprintf(fp, "%d\t%d\t%.10f\t%d\n", ++l, file->lines[id].i1, file->lines[id].d2, file->lines[id].i3);
		}
		free_idiLineFile(file);
	}
*/


	//addWorktoThreadPool(writeContinuousi4LineFileHT, file);
	//init_DirectTimeNet(file);
	//addWorktoThreadPool(verifyDTNet, NULL);
	
/*
	//read data file, create net;
	struct iiLineFile *file=create_iiLineFile(datafilename);
	//addWorktoThreadPool(writeContinuousNetFileHT, file);

	buildDNet(file);
	addWorktoThreadPool(verifyDNet, NULL);
	free_iiLineFile(file);

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
		args_thread[i].loopNum = 3000;
		addWorktoThreadPool(dnet_spread, args_thread+i);
	}
*/
	destroyThreadPool();

//	free_innLineFile(ISs);
//	freeDNet();
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
