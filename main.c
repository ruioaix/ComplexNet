#include <stdio.h>
#include <time.h>
#include "inc/complexnet_config.h" //for config
#include "inc/complexnet_file.h" //for readFileLBL;
#include "inc/complexnet_dnet.h" //for buildDNet;

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);

	//argv,argc process;
	char *datafilename="data/data";
	char *originfilename="data/origin";
	if (argc==2) datafilename=argv[1];
	if (argc==3) {
		datafilename=argv[1];
		originfilename=argv[2];
	}

	//read data file, create net;
	struct NetFile *file=readFileLBL(datafilename);
	struct DirectNet *dnet=buildDNet(file);

	//read origin file, create IS;
	struct InfectSourceFile *IS=readISfromFile(originfilename);
	int i;
	vttype j;
	for (i=0; i<IS->ISsNum; ++i) {
		vttype num=IS->ISs[i].num;
		for (j=0; j<num; ++j) {
			printf("%d, ",IS->ISs[i].vt[j]);
		}
		printf("\n");
	}

	//IS, NET, infectRate, loopNum
	//dnet_spread(ISs, dnet, 0.9, 0, 20);
	//printf("spread steps: %d\n", spread);

	//free;
	//free(IS->vt);
	free(IS);
	free(file->lines);
	free(file);
	freeDNet(dnet);
	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
