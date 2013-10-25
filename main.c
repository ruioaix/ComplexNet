//#define NDEBUG  //for assert
#include "inc/complexnet_file.h" //for readFileLBL;
#include "inc/complexnet_net.h" //for buildDNet;
#include "inc/complexnet_error.h"
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);

	struct iiLineFile *file = create_iiLineFile("data/net_power4941.txt");
	create_Net(file);
	verifyNet(NULL);
	free_iiLineFile(file);
	

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
