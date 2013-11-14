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
#include <string.h>

//char *get_Next_Snapshot(struct i3LineFile *file, struct Net *net, int *infect_source);
int get_Next_Snapshot(struct i3LineFile *file, struct Net *net, int *infect_source, char *stat);

int main(int argc, char **argv)
{
	if (argc != 8) {
		printf("main error\n\twrong args number: %d\n", argc);
		return -1;
	}
	char *pEnd;
	int T = strtol(argv[1], &pEnd, 10);
	double infect_rate = strtod(argv[2], &pEnd);
	double recover_rate = strtod(argv[3], &pEnd);

	char *net_filename = argv[4];
	char *eye_filename = argv[5];
	char *snapshot_filename = argv[6];

	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	
	//net
	struct iiLineFile *file = create_iiLineFile(net_filename);
	//struct iiLineFile *file = create_iiLineFile("data/amazon/net_amazonungraph0.000.txt");
	struct Net *net = create_Net(file);
	
	//eye
	struct iLineFile *eye_nodes = create_iLineFile(eye_filename);
	//struct iLineFile *eye_nodes = create_iLineFile("data/amazon/eye_amazon_rndNon-0.01.txt");

	//snapshot
	struct i3LineFile *snapshot = create_i3LineFile(snapshot_filename);
	//struct i3LineFile *snapshot = create_i3LineFile("data/amazon/snapshot_amazonInstanceAllnew0.60.txt");

	//for output
	double *realE = malloc((net->maxId + 1)*sizeof(double));
	assert(realE != NULL);
	int  *rank = malloc((net->maxId + 1)*sizeof(int));
	assert(rank != NULL);
	char *sign = calloc((net->maxId + 1), sizeof(int));
	assert(sign != NULL);
	
	FILE *fp;
	fp = fopen(argv[7], "w");
	int j, i;

	//get real_infect_source information
	printf("Pareparing...\n");fflush(stdout);
	int real_infect_source;
	char *stat = calloc(net->maxId + 1, sizeof(char));
	assert(stat != NULL);
	int ok = get_Next_Snapshot(snapshot, net, &real_infect_source, stat);
	double *PSIR = net_dmp_is(net, real_infect_source, T, infect_rate, recover_rate);
	while(ok) {
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
		ok = get_Next_Snapshot(snapshot, net, &real_infect_source, stat);
		free(PSIR);
		PSIR = net_dmp_is(net, real_infect_source, T, infect_rate, recover_rate);
	}
	free(PSIR);
	printf("Pareparing done\n");fflush(stdout);

	//rank
	
	printf("computing:\n");fflush(stdout);
	//for (i=0; i<net->maxId + 1; ++i) {
	for (i=0; i<100; ++i) {
		if (i%100 == 0) {printf("%.5f %d\r", ((double)i)/net->maxId, i);fflush(stdout);}
		if (net->count[i]) {
			double *PSIR = net_dmp_is(net, i, T, infect_rate, recover_rate);
			int real_infect_source;
			ok = get_Next_Snapshot(snapshot, net, &real_infect_source, stat);
			while(ok) {
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
				ok = get_Next_Snapshot(snapshot, net, &real_infect_source, stat);
			}
			free(PSIR);
		} 
	}

	double ave=0;
	int count = 0;
	for (i=0; i<net->maxId + 1; ++i) {
		if (sign[i]) {
			fprintf(fp, "%d, %d\n", i, rank[i]);
			ave += rank[i];
			++count;
		}
	}
	fclose(fp);
	printf("ave: %f\n", ave/count);

	free(rank);
	free(realE);
	free(sign);
	free(stat);
	free_i3LineFile(snapshot);
	free_iLineFile(eye_nodes);
	free_Net(net);
	free_iiLineFile(file);
	
	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}


//request: snapshot file is related to net file.
//request: in snapshot file, only 1&2 is recorded, 0 is not.
int get_Next_Snapshot(struct i3LineFile *file, struct Net *net, int *infect_source, char *stat) {
	static int snapshot_z = 0;
	if (snapshot_z == file->linesNum) {
		snapshot_z = 0;
		return 0;
	}	

	//calloc(net->maxId + 1, sizeof(char));
	memset(stat, 0, (net->maxId + 1)*sizeof(char));

	int i=snapshot_z;
	*infect_source = file->lines[i].i1;
	while (file->lines[i].i1 == file->lines[snapshot_z].i1 && i<file->linesNum) {
		stat[file->lines[i].i2] = file->lines[i].i3;
		++i;
	}
	snapshot_z = i;
	return 1;
}
