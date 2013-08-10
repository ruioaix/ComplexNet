#define NDEBUG 

#include <stdio.h>
#include <time.h>
#include <limits.h>
#include "inc/complexnet_file.h" //for readFileLBL;

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);

	//argv,argc process;
	char *datafilename="data/data";
	//char *originfilename="data/origin1";
	if (argc==2) datafilename=argv[1];
	if (argc==3) {
		datafilename=argv[1];
		//originfilename=argv[2];
	}

	//read data file;
	struct NetFile *file=readFileLBL(datafilename);

	//create net;
	//enum isDirect isD=directed;
	//struct Net *net=createNet(edList, edNum, VTREALNUM, isD);
	//free fileContent;
	free(file);

	//work area;

	//free net;
	//freeNet(net);
	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
