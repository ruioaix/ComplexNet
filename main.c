#define NDEBUG  //for assert
#include "inc/complexnet_file.h" //for readFileLBL;
#include "inc/complexnet_net.h" //for buildDNet;
#include "inc/complexnet_net_pspipr.h" //for buildDNet;
#include "inc/complexnet_net_snapshot.h" //for buildDNet;
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

	struct iid3LineFile *pspipr = create_iid3LineFile("data/PS_PI_PR_Time_10.txt");
	create_Net_PSPIPR(pspipr);
	struct Net_PSPIPR *net_pspipr = get_Net_PSPIPR();

	struct i3LineFile *snapshot = create_i3LineFile("data/snapshot_InstanceAllnew0.60.txt");
	create_Net_SNAPSHOT(snapshot);
	struct Net_SNAPSHOT *net_snapshot = get_Net_SNAPSHOT();


	int i;
	int j;
	for (i=0; i < net_snapshot->maxId+1; ++i) {
		if (net_snapshot->count[i]>0) {
			for (j=0; j<eye_nodes->linesNum; ++j) {
				int eye = eye_nodes->lines[j].i1;
				int status = find_Net_SNAPSHOT_status(i, eye);
				double P = find_Net_PSPIPR(i, eye, status);
			}
			
		}
	}

	//free_iiLineFile(file);
	//free_Net();
	

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
