#define NDEBUG 

#include <stdio.h>
#include <time.h>
#include "inc/complexnet_file.h" //for readFileLBL;
#include "inc/complexnet_dnet.h" //for buildDNet;

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);

	//argv,argc process;
	char *datafilename="data/data";
	char *originfilename="data/origin1";
	if (argc==2) datafilename=argv[1];
	if (argc==3) {
		datafilename=argv[1];
		originfilename=argv[2];
	}

	//read data file, create net;
	struct NetFile *file=readFileLBL(datafilename);
	struct DirectNet *dnet=buildDNet(file);

	//read origin file, create IS;
	struct InfectSource *IS=readISfromFile(originfilename);

	//IS, NET, infectRate, loopNum
	dnet_spread_touch_all(IS, dnet, 0.5, 20);
	//printf("spread steps: %d\n", spread);

	//analysis dnet to get information you want.
	//TODO

	//free;
	free(IS->vt);
	free(IS);
	free(file->lines);
	free(file);
	freeDNet(dnet);
	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
