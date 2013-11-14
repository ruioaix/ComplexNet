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

char *get_Next_Snapshot(struct i3LineFile *file, struct Net *net, int *infect_source);

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	
	//net
	//struct iiLineFile *file = create_iiLineFile("data/youtube/net_youtube_ungraph_0.000.txt");
	//struct iiLineFile *file = create_iiLineFile("data/net_power4941.txt");
	struct iiLineFile *file = create_iiLineFile("data/amazon/net_amazonungraph0.000.txt");
	struct Net *net = create_Net(file);
	//net_dmp(net, 10, 0.6, 0.5);
	
	//eye
	//struct iLineFile *eye_nodes = create_iLineFile("data/eye_rndNon_0.05.txt");
	struct iLineFile *eye_nodes = create_iLineFile("data/amazon/eye_amazon_rndNon-0.01.txt");

	//snapshot
	//struct i3LineFile *snapshot = create_i3LineFile("data/snapshot_InstanceAllnew0.60.txt");
	struct i3LineFile *snapshot = create_i3LineFile("data/amazon/snapshot_amazonInstanceAllnew0.60.txt");
	//struct i3LineFile *snapshot = create_i3LineFile("data/youtube/snapshot_youtube_InstaceAllnew0.60.txt");
	//iint snapshotNum = geti1num_i3LineFile(snapshot);

	double *realE = malloc((net->maxId + 1)*sizeof(double));
	assert(realE != NULL);
	int  *rank = malloc((net->maxId + 1)*sizeof(int));
	assert(rank != NULL);
	char *sign = calloc((net->maxId + 1), sizeof(int));
	assert(sign != NULL);
	

	FILE *fp;
	fp = fopen("Results/de_infectsource_rank.txt", "w");
	int Rank=1;
	int j, k, i;

	int guess_infect_source = 1;
	int real_infect_source;
	char *stat = get_Next_Snapshot(snapshot, net, &real_infect_source);
	double *PSIR = net_dmp_is(net, real_infect_source, 10, 0.6, 0.5);
	while(stat) {
		double E = 0;
		for (j=0; j<eye_nodes->linesNum; ++j) {
			int eye = eye_nodes->lines[j].i1;
			int status = stat[eye];
			double P = *(PSIR + (net->maxId + 1)*status + eye);
			if (P == 0 ) {
				E += 1000;
				continue;
			}
			E -= log(P);
		}
		realE[real_infect_source] = E;
		rank[real_infect_source] = 0;
		sign[real_infect_source] = 1;
		free(stat);
		stat = get_Next_Snapshot(snapshot, net, &real_infect_source);
		free(PSIR);
		PSIR = net_dmp_is(net, real_infect_source, 10, 0.6, 0.5);
	}
	free(PSIR);

	for (i=0; i<net->maxId + 1; ++i) {
	//for (i=0; i<100; ++i) {
		if (i%100 == 0) {printf("%d\n", i);fflush(stdout);}
		if (net->count[i]) {
			double *PSIR = net_dmp_is(net, i, 10, 0.6, 0.5);
			int real_infect_source;
			char *stat = get_Next_Snapshot(snapshot, net, &real_infect_source);
			while(stat) {
				double E = 0;
				for (j=0; j<eye_nodes->linesNum; ++j) {
					int eye = eye_nodes->lines[j].i1;
					int status = stat[eye];
					double P = *(PSIR + (net->maxId + 1)*status + eye);
					if (P == 0 ) {
						E += 1000;
						continue;
					}
					E -= log(P);
				}
				if (E < realE[real_infect_source]) {
					++rank[real_infect_source];
				}
				free(stat);
				stat = get_Next_Snapshot(snapshot, net, &real_infect_source);
			}
			free(PSIR);
		} 
	}

	double ave=0;
	int xx = 0;
	for (i=0; i<net->maxId + 1; ++i) {
		if (sign[i]) {
			fprintf(fp, "%d, %d\n", i, rank[i]);
			ave += rank[i];
			++xx;
		}
	}
	fclose(fp);
	printf("ave: %f\n", ave/xx);

	free_iiLineFile(file);
	free(rank);
	free(realE);
	free(sign);
	free_i3LineFile(snapshot);
	free_iLineFile(eye_nodes);
	//free(PS);
	free_Net(net);
	
	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}


//request: snapshot file is related to net file.
//request: in snapshot file, only 1&2 is recorded, 0 is not.
char *get_Next_Snapshot(struct i3LineFile *file, struct Net *net, int *infect_source) {
	static int snapshot_z = 0;
	if (snapshot_z == file->linesNum) {
		snapshot_z = 0;
		return NULL;
	}	
	char *stat = calloc(net->maxId + 1, sizeof(char));
	assert(stat != NULL);

	int i=snapshot_z;
	*infect_source = file->lines[i].i1;
	while (file->lines[i].i1 == file->lines[snapshot_z].i1 && i<file->linesNum) {
		stat[file->lines[i].i2] = file->lines[i].i3;
		++i;
	}
	snapshot_z = i;
	return stat;
}
