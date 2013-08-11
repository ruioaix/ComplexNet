#define NDEBUG 

#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <math.h>
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

	//read data file;
	struct NetFile *file=readFileLBL(datafilename);
	struct InfectSource *IS=readISfromFile(originfilename);
	//create net;
	struct DirectNet *dnet=buildDNet(file);
	printf("%d\n", dnet->countMax);

	buildIStoDNet(IS, dnet);
	//struct InfectSource *IS=buildIS(isfile);
	//IS->num = 2;
	//IS->vt = malloc(2*sizeof(idtype));
	//IS->vt[0] = 11;
	//IS->vt[1] = 12;
	//dnet->status[11]=1;
	//dnet->status[12]=1;

	int spread = spread_touch_all(IS, dnet, 0.5);
	printf("%d\n", spread);

	//free;
	free(file->lines);
	free(file);
	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
