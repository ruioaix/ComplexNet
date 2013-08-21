//#define NDEBUG
#include <stdio.h>
#include <time.h>
#include "inc/complexnet_file.h" //for readFileLBL;
#include "inc/complexnet_dnet.h" //for buildDNet;
#include "inc/complexnet_hashtable.h" //for buildDNet;

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
	

	//read data file, create net;
	struct NetFile *file=readNetFile(datafilename);

	pthread_t tid;
	pthread_create(&tid, NULL, getInforHT, file);
	
	struct DirectNet *dnet=buildDNet(file);

	//read origin file, create IS;
	struct InfectSourceFile *ISs=readAllISfromFile(originfilename);

	//ISs, NET, infectRate, touchparam, loopNum, Thread_max.
	dnet_spread(ISs, dnet, 0.9, 0, 2, 4);

	//free;
	//free(IS->vt);
	freeISFile(ISs);
	pthread_join(tid, NULL);
	freeNetFile(file);
	freeDNet(dnet);
	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
