#include <stdio.h>
#include <time.h>
#include <limits.h>
#include "inc/complexnet_file.h" //for readFileLBL;

#define VTREALNUM 3210678

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
	long edNum, vtMaxId=-1, vtMinId=LONG_MAX;
	int partNum=4;
	struct LineInfo	*edList=readFileLBL(datafilename, &edNum, &vtMaxId, &vtMinId, partNum);

	//create net;
	//enum isDirect isD=directed;
	//struct Net *net=createNet(edList, edNum, VTREALNUM, isD);
	//free fileContent;
	free(edList);

	//work area;

	//free net;
	//freeNet(net);
	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
