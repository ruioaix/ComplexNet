#define NDEBUG  //for assert
#include "inc/complexnet_linefile.h" //for readFileLBL;
#include "inc/complexnet_net.h" //for buildDNet;
#include "inc/complexnet_net_pspipr.h" //for buildDNet;
#include "inc/complexnet_net_snapshot.h" //for buildDNet;
#include "inc/complexnet_error.h"
#include "inc/complexnet_sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	int i;
	int j;
	int k;

	//struct iiLineFile *file = create_iiLineFile("data/net_power4941.txt");
	//create_Net(file);
	//verifyNet(NULL);
	//net_dmp(10, 0.6, 0.5);
	//exit(0);

	//struct iLineFile *eye_nodes = create_iLineFile("data/eye_rnd_0.05.txt");
	struct iLineFile *eye_nodes = create_iLineFile("data/eye_rnd_Non_0.05.txt");
	//struct iLineFile *eye_nodes = create_iLineFile("data/eye_degreeNon_0.05-1.txt");

	struct iid3LineFile *pspipr = create_iid3LineFile("data/PS_PI_PR_Time_10.txt");
	create_Net_PSPIPR(pspipr);
	struct Net_PSPIPR *net_pspipr = get_Net_PSPIPR();

	struct i3LineFile *snapshot = create_i3LineFile("data/snapshot_InstanceAllnew0.60.txt");
	create_Net_SNAPSHOT(snapshot);
	struct Net_SNAPSHOT *net_snapshot = get_Net_SNAPSHOT();

	FILE *fp;
	fp = fopen("Results/de_infectsource_rank.txt", "w");
	double E, E_i;
	int Rank=1;

	int ave = 0;
	//for every snapshot has a Rank output.
	for (i=1; i < net_snapshot->maxId+1; ++i) {
		if (i%1000 == 0) printf("%d\n", i);

		E_i = 0;
		for (j=0; j<eye_nodes->linesNum; ++j) {
			int eye = eye_nodes->lines[j].i1;
			int status = net_snapshot->stat[i][eye];
			double *Pp = net_pspipr->psir[i][eye];
			double P;
			if (Pp != NULL) {
				P = Pp[status];
			}
			else { 
				P = status==0?1:0;
			}
			if (P == 0 ) {
				E_i += INT_MAX;
				continue;
			}
			E_i -= log(P);
		}

		Rank = 0;
		for (k=1; k<net_pspipr->maxId+1 && k != i; ++k) {
			E = 0;
			for (j=0; j<eye_nodes->linesNum; ++j) {
				int eye = eye_nodes->lines[j].i1;
				int status = net_snapshot->stat[k][eye];
				double *Pp = net_pspipr->psir[k][eye];
				double P;
				if (Pp != NULL) {
					P = Pp[status];
				}
				else { 
					P = status==0?1:0;
				}
				if (P == 0 ) {
					E = E_i + 1;
					break;
				}
				E -= log(P);
			}
			if (E < E_i) {
				++Rank;
			}
		}
		fprintf(fp, "%d, %d\n", i, Rank);
		ave += Rank;
		//for (k = 0; k<net_pspipr->maxId+1; ++k) {
		//	printf("%d, %f\n", k, Rank[k]);
		//}
		//quick_sort_double_index(Rank, 0, net_pspipr->maxId, Rank_index);
		//for (k=0; k<net_pspipr->maxId+1; ++k) {
		//	if (Rank_index[k] == i) {
		//		//printf("%d, %d\n", i, k);
		//		break;
		//	}
		//}
	}
	printf("%f\n", (double)ave/(double)net_pspipr->maxId);

	//free_iiLineFile(file);
	//free_Net();
	
	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
