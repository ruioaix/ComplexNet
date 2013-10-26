#define NDEBUG  //for assert
#include "inc/complexnet_file.h" //for readFileLBL;
#include "inc/complexnet_net.h" //for buildDNet;
#include "inc/complexnet_net_pspipr.h" //for buildDNet;
#include "inc/complexnet_error.h"
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);

	//struct iiLineFile *file = create_iiLineFile("data/net_power4941.txt");
	//create_Net(file);
	//verifyNet(NULL);
	//net_dmp(10);

	struct iLineFile *eye_nodes = create_iLineFile("data/eye_rnd_0.05.txt");
	printf("%ld\n", eye_nodes->linesNum);
	struct iid3LineFile *pspipr = create_iid3LineFile("data/PS_PI_PR_Time_10.txt");
	printf("%ld\n", pspipr->linesNum);
	create_Net_PSPIPR(pspipr);



	//free_iiLineFile(file);
	//free_Net();
	

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
