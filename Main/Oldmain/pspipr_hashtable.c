#define NDEBUG  //for assert
#include "inc/linefile/ilinefile.h" //for readFileLBL;
#include "inc/linefile/iilinefile.h" //for readFileLBL;
#include "inc/linefile/i3linefile.h" //for readFileLBL;
#include "inc/linefile/iid3linefile.h" //for readFileLBL;
#include "inc/compact/net.h" //for buildDNet;
#include "inc/matrix/pspipr.h" //for buildDNet;
#include "inc/matrix/snapshot.h" //for buildDNet;
#include "inc/utility/error.h"
#include "inc/utility/sort.h"
#include "inc/hashtable/dmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <assert.h>

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	int i;
	int j;
	int k;

	struct iiLineFile *file = create_iiLineFile("data/net_power4941.txt");
	//struct iiLineFile *file = create_iiLineFile("data/youtube/net_youtube_ungraph_0.000.txt");

	//struct Net *neti = create_Net(file);
	////print_Net(net, "Results/youtube_net");
	////exit(0);
	////verifyNet(net);
	//net_dmp(neti, 10, 0.6, 0.5);
	//t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);

	//exit(0);

	struct HashTable_DMP *ht = create_HashTable_DMP(file->linesNum);
	int infect_source = 1;
	init_HashTable_DMP(ht, file, infect_source);
	print_ENum_HashTable_DMP(ht, "Results/enum_ht");
	//exit(0);

	struct Net *net = create_Net(file);
	//print_Net(net, "Results/youtube_net");
	net_dmp(net, 10, 0.6, 0.5);

	FILE *fp1 = fopen("Results/hashtable_out", "w");
	fileError(fp1, "main");

	double *PS = malloc((net->maxId+1)*sizeof(double));
	assert(PS != NULL);
	double *PI = malloc((net->maxId+1)*sizeof(double));
	assert(PI != NULL);
	double *PR = malloc((net->maxId+1)*sizeof(double));
	assert(PR != NULL);

	for (j=1; j<net->maxId + 1; ++j) {
		clean_HashTable_DMP(ht, j);
		dmp(ht, j, 0.6, 0.5, 10, net, PS, PI, PR);
		if (!PS) continue;
		
		//struct HashElement_DMP *he = getHEfromHT_DMP(ht, 1, 387);
		//printf("%d,%d,%f,%f,%f,%f\n", he->i1, he->i2, he->theta, he->phi, he->p1, he->p2);

		for (i=1; i<net->maxId + 1; ++i) {
			if ((PS[i] != 1 || PI[i] !=0 || PR[i] != 0) ) {
				fprintf(fp1, "%d, %d, %0.17f, %0.17f, %0.17f\n", j, i, PS[i], PI[i], PR[i]);
			}
		}
		if (j%1000 == 0) {
			printf("%d\n", j);fflush(stdout);
		}
	}
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);

	/*
	//struct iLineFile *eye_nodes = create_iLineFile("data/eye_rnd_0.05.txt");
	struct iLineFile *eye_nodes = create_iLineFile("data/eye_rndNon_0.05.txt");
	//struct iLineFile *eye_nodes = create_iLineFile("data/youtube/eye_youtube_rndNon_0.01.txt");
	//print_iLineFile(eye_nodes, "Results/youtube_eye");
	//struct iLineFile *eye_nodes = create_iLineFile("data/eye_degreeNon_0.05-1.txt");

	struct iid3LineFile *pspipr = create_iid3LineFile("data/PS_PI_PR_Time_10.txt");
	//print_iid3LineFile(pspipr, "Results/PS_PI_PR_output");

	struct PSPIPR *net_pspipr = create_PSPIPR(pspipr);
	//print_Net_PSPIPR("Results/PS_PI_PR_net_output");

	struct i3LineFile *snapshot = create_i3LineFile("data/snapshot_InstanceAllnew0.60.txt");
	//struct i3LineFile *snapshot = create_i3LineFile("data/youtube/snapshot_youtube_InstaceAllnew0.60.txt");
	//print_i3LineFile(snapshot, "Results/snapshot_instances_output");
	//exit(0);
	struct Snapshot * net_snapshot = create_Snapshot(snapshot);
	//print_Snapshot("Results/snapshot_net_output");

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
					//printf("%d, %d, %f, %d\n", i, eye, P, status);
			if (P == 0 ) {
				E_i += 1000;
				continue;
			}
			E_i -= log(P);
		}
		//exit(0);
		//printf("%d, %e\n", i, E_i);

		Rank = 0;
		for (k=1; k<net_pspipr->maxId+1; ++k) {
			if (k != i) {
				E = 0;
				for (j=0; j<eye_nodes->linesNum; ++j) {
					int eye = eye_nodes->lines[j].i1;
					int status = net_snapshot->stat[i][eye];
					double *Pp = net_pspipr->psir[k][eye];
					double P;
					if (Pp != NULL) {
						P = Pp[status];
					}
					else { 
						P = status==0?1:0;
					}
					//printf("%d, %d, %f, %d\n", k, eye, P, status);
					if (P == 0 ) {
						E += 1000;
						//break;
					}
					E -= log(P);
					//printf("%d, %0.17f\n", k, E);fflush(stdout);
				}
				if (E < E_i) {
					++Rank;
				}
				//exit(0);
			//printf("%d, %e\n", k, E);
			}
		}
		//exit(0);
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
	*/
	//free_iiLineFile(file);
	//free(PS);
	//free_Net(net);
	
	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
