#define NDEBUG  //for assert
#include "inc/linefile/ilinefile.h" //for readFileLBL;
#include "inc/linefile/iilinefile.h" //for readFileLBL;
#include "inc/linefile/i3linefile.h" //for readFileLBL;
#include "inc/linefile/iid3linefile.h" //for readFileLBL;
#include "inc/compact/net.h" //for buildDNet;
#include "inc/compact/iidnet.h" //for buildDNet;
#include "inc/compact/bip2.h" //for buildDNet;
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

int snapshot_z = 0;
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
	//struct iLineFile *eye_nodes = create_iLineFile(eye_filename);
	//struct iLineFile *eye_nodes = create_iLineFile("data/amazon/eye_amazon_rndNon-0.01.txt");
	struct iiLineFile *eye_nodes_file = create_iiLineFile(eye_filename);
	struct Bip2 *eye_nodes = create_Bip2(eye_nodes_file, 1);

	//snapshot
	struct i3LineFile *snapshot = create_i3LineFile(snapshot_filename);
	//struct i3LineFile *snapshot = create_i3LineFile("data/amazon/snapshot_amazonInstanceAllnew0.60.txt");
	
	int i;

	//for output
	double **realE = malloc((eye_nodes->maxId + 1)*sizeof(void *));
	assert(realE != NULL);
	for (i=0; i<eye_nodes->maxId + 1; ++i) {
		realE[i] = calloc((net->maxId + 1),sizeof(double));
		assert(realE[i] != NULL);
	}

	int  **rank = malloc((eye_nodes->maxId + 1)*sizeof(void *));
	assert(rank != NULL);
	for (i=0; i<eye_nodes->maxId + 1; ++i) {
		rank[i] = calloc((net->maxId + 1),sizeof(int));
		assert(rank[i] != NULL);
	}

	char **sign = malloc((eye_nodes->maxId + 1)*sizeof(void *));
	assert(sign != NULL);
	for (i=0; i<eye_nodes->maxId + 1; ++i) {
		sign[i] = calloc((net->maxId + 1),sizeof(char));
		assert(sign[i] != NULL);
	}
	
	FILE *fp;
	fp = fopen(argv[7], "w");
	int j;

	//get real_infect_source information
	printf("Pareparing...\n");fflush(stdout);
	int real_infect_source;
	char *stat = calloc(net->maxId + 1, sizeof(char));
	assert(stat != NULL);
	int ok = get_Next_Snapshot(snapshot, net, &real_infect_source, stat);
	double *PSIR = net_dmp_is(net, real_infect_source, T, infect_rate, recover_rate);
	while(ok) {
		for (i=0; i<eye_nodes->maxId + 1; ++i) {
			if (eye_nodes->count[i]) {
				double E = 0;
				for (j=0; j<eye_nodes->count[i]; ++j) {
					int eye = eye_nodes->id[i][j];
					int status = stat[eye];
					double P = *(PSIR + (net->maxId + 1)*status + eye);
					if (P == 0 ) {
						E += 1000;
						continue;
					}
					E -= log(P);
				}
				realE[i][real_infect_source] = E;
				rank[i][real_infect_source] = 0;
				sign[i][real_infect_source] = 1;
				ok = get_Next_Snapshot(snapshot, net, &real_infect_source, stat);
				free(PSIR);
				PSIR = net_dmp_is(net, real_infect_source, T, infect_rate, recover_rate);
			}
		}
	}
	free(PSIR);
	printf("Pareparing done\n");fflush(stdout);

	//rank
	
	printf("computing:\n");fflush(stdout);
	int ii;
	for (i=0; i<net->maxId + 1; ++i) {
	//for (i=0; i<100; ++i) {
		if (i%100 == 0) {printf("%.5f %d\r", ((double)i)/net->maxId, i);fflush(stdout);}
		if (net->count[i]) {
			double *PSIR = net_dmp_is(net, i, T, infect_rate, recover_rate);
			int real_infect_source;
			ok = get_Next_Snapshot(snapshot, net, &real_infect_source, stat);
			while(ok) {
				for (ii=0; ii<eye_nodes->maxId + 1; ++ii) {
					if (eye_nodes->count[ii]) {
						double E = 0;
						for (j=0; j<eye_nodes->count[ii]; ++j) {
							int eye = eye_nodes->id[ii][j];
							int status = stat[eye];
							double P = *(PSIR + (net->maxId + 1)*status + eye);
							if (P == 0 ) {
								E += 1000;
								continue;
							}
							E -= log(P);
						}
						if (E < realE[ii][real_infect_source]) {
							++rank[ii][real_infect_source];
						}
						ok = get_Next_Snapshot(snapshot, net, &real_infect_source, stat);
					}
				}
			}
			free(PSIR);
		} 
	}

	double *ave = calloc(eye_nodes->maxId + 1, sizeof(double));
	assert(ave != NULL);
	int *count = calloc(eye_nodes->maxId+1, sizeof(int));
	assert(count != NULL);
	for (ii=0; ii<eye_nodes->maxId + 1; ++ii) {
		if (eye_nodes->count[ii]) {
			for (i=0; i<net->maxId + 1; ++i) {
				if (sign[ii][i]) {
					fprintf(fp, "%d, %d\n", i, rank[ii][i]);
					ave[ii] += rank[ii][i];
					++count[ii];
				}
			}
		}
	}
	fclose(fp);
	for (ii=0; ii<eye_nodes->maxId + 1; ++ii) {
		if (eye_nodes->count[ii]) {
			printf("eye: %d, ave: %f\n", ii, ave[ii]/count[ii]);
		}
	}

	for (i=0; i<eye_nodes->maxId + 1; ++i) {
		free(rank[i]);
		free(realE[i]);
		free(sign[i]);
	}
	free(rank);
	free(realE);
	free(sign);
	free(stat);
	free(ave);
	free(count);
	free_i3LineFile(snapshot);
	free_iiLineFile(eye_nodes_file);
	free_Bip2(eye_nodes);
	free_Net(net);
	free_iiLineFile(file);
	
	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}


//request: snapshot file is related to net file.
//request: in snapshot file, only 1&2 is recorded, 0 is not.
int get_Next_Snapshot(struct i3LineFile *file, struct Net *net, int *infect_source, char *stat) {
	if (snapshot_z == file->linesNum) {
		snapshot_z = 0;
		return 0;
	}	

	//calloc(net->maxId + 1, sizeof(char));
	memset(stat, 0, (net->maxId + 1)*sizeof(char));

	int i=snapshot_z;
	*infect_source = file->lines[i].i1;
	while (i<file->linesNum &&file->lines[i].i1 == file->lines[snapshot_z].i1) {
		stat[file->lines[i].i2] = file->lines[i].i3;
		++i;
	}
	snapshot_z = i;
	return 1;
}
