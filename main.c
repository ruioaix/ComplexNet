#define NDEBUG 

#include <stdio.h>
#include <time.h>
#include <limits.h>
#include "inc/complexnet_file.h" //for readFileLBL;
#include "inc/complexnet_dnet.h" //for buildDNet;
#include "inc/complexnet_random.h"

int main(void)
{
    int i;
    //unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    //init_by_array_MersenneTwister(init, length);
    printf("1000 outputs of genrand_int32()\n");
    for (i=0; i<1000; i++) {
      printf("%10lu ", genrand_int32());
      if (i%5==4) printf("\n");
    }
    printf("\n1000 outputs of genrand_real2()\n");
    for (i=0; i<1000; i++) {
      printf("%10.8f ", genrand_real2());
      if (i%5==4) printf("\n");
    }
    return 0;
}

/*
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
	struct DirectNet *dnet=buildDNet(file);
	
	//free;
	free(file->lines);
	free(file);
	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
*/
