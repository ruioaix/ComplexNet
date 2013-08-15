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
	char *originfilename="data/origin1";
	if (argc==2) datafilename=argv[1];
	else if (argc==3) {
		datafilename=argv[1];
		originfilename=argv[2];
	}
	else if (argc>3) {
		isError("two much args");
	}
	

	//read data file, create net;
	struct NetFile *file=readFileLBL(datafilename);
	struct DirectNet *dnet=buildDNet(file);

	//read origin file, create IS;
	struct InfectSourceFile *IS=readISfromFile(originfilename);

	//ISs, NET, infectRate, loopNum
	dnet_spread(IS, dnet, 0.9, 0, 20);

	//free;
	//free(IS->vt);
	freeISFile(IS);
	freeNetFile(file);
	freeDNet(dnet);
	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
