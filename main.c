//#define NDEBUG  //for assert
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "inc/linefile/iilinefile.h"
#include "inc/linefile/i3linefile.h"
#include "inc/linefile/iidlinefile.h"
#include "inc/compact/bip2.h"
#include "inc/utility/sort.h"
#include "inc/utility/error.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	char *netfilename;
	if (argc == 1) {
		netfilename = "data/movielen/movielen2";
	}
	else if (argc == 2) {
		netfilename = argv[1];
		//char *pEnd;
		//maxscore = strtol(argv[2], &pEnd, 10);
		//stepbegin = strtod(argv[4], &pEnd);
	}
	else {
		printf("wrong argc\n");
		return 0;
	}

	//get netfile, build net.
	struct iiLineFile *netfile = create_iiLineFile(netfilename);
	struct Bip2 *bipi1 = create_Bip2(netfile, 1);
	struct Bip2 *bipi2 = create_Bip2(netfile, 0);
	free_iiLineFile(netfile);

	struct iidLineFile *similarity_file = similarity_realtime_Bip2(bipi1, bipi2, 1);
	int i;
	long j;
	//int stepNum = 100;
	//int *fenbu = calloc(stepNum, sizeof(int));
	//for (j=0; j<similarity_file->linesNum; ++j) {
	//	double sim = similarity_file->lines[j].d3;
	//	int index = floor(sim*stepNum);
	//	++fenbu[index];
	//}
	//FILE *fp = fopen("Results/fenbu", "w");
	//fileError(fp, "main2");
	//for (i=0; i<stepNum; ++i) {
	//	fprintf(fp, "%f, %f\n", (double)i/stepNum, (double)fenbu[i]/similarity_file->linesNum);
	//}
	//return 0;

	struct iidNet *similarity = create_iidNet(similarity_file);
	free_iidLineFile(similarity_file);

	int *M = malloc((similarity->maxId + 1)*sizeof(int));
	assert(M != NULL);
	int *Mfb = malloc((similarity->maxId + 1)*sizeof(int));
	assert(Mfb != NULL);

	int k;
	for (k=0; k<20; ++k) {
		double srate = k*0.05;
		memset(M, 0, (similarity->maxId + 1)*sizeof(int));
		memset(Mfb, 0, (similarity->maxId + 1)*sizeof(int));
		for (i=0; i<similarity->maxId + 1; ++i) {
			if (similarity->count[i]) {
				for (j=0; j<similarity->count[i]; ++j) {
					M[i] += similarity->d3[i][j] > srate ?1:0;
				}
			}
		}
		int total=0;
		for (i=0; i<similarity->maxId + 1; ++i) {
			if (M[i]) {
				++Mfb[M[i]];
				++total;
			}
		}
		if (total) {
			char filename[1000];
			sprintf(filename, "Results/Mfb%.2f", srate);
			FILE *fp = fopen(filename, "w");
			fileError(fp, "main");
			for (i=0; i<similarity->maxId + 1; ++i) {
			//for (i=0; i<20; ++i) {
				if (Mfb[i]) {
					fprintf(fp, "%d, %.17f\n", i, (double)Mfb[i]/total);
				}
			}
			fclose(fp);
		}
	}



	//char filename[1000];
	//double *onesim = malloc((similarity->maxId +1)*sizeof(double));
	//assert(onesim!=NULL);
	////for (i=0; i<similarity->maxId + 1; ++i) {
	//int begin = 0;
	//for (i=begin; i<begin + 20; ++i) {
	//	if (similarity->count[i]) {
	//		sprintf(filename, "Results/sim%d", i);
	//		FILE *fp = fopen(filename, "w");
	//		fileError(fp, "main");

	//		memcpy(onesim, similarity->d3[i], similarity->count[i]*sizeof(double));
	//		qsort_d_desc(onesim, 0, similarity->count[i]-1);
	//		for(j=0; j<similarity->count[i]; ++j) {
	//			fprintf(fp, "%ld, %.17f\n", j, onesim[j]);
	//		}

	//		fclose(fp);
	//	}
	//}


	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
