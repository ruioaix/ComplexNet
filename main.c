//#define NDEBUG  //for assert
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "inc/linefile/iilinefile.h"
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
		netfilename = "data/movielen/movielens.txt";
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
	struct iidNet *similarity = create_iidNet(similarity_file);
	//print_iidNet(similarity, "Results/simm");
	//return 0;
	free_iidLineFile(similarity_file);

	int i;
	long j;
	char filename[1000];
	double *onesim = malloc((similarity->maxId +1)*sizeof(double));
	assert(onesim!=NULL);
	//for (i=0; i<similarity->maxId + 1; ++i) {
	int begin = 0;
	for (i=begin; i<begin + 20; ++i) {
		if (similarity->count[i]) {
			sprintf(filename, "Results/sim%d", i);
			FILE *fp = fopen(filename, "w");
			fileError(fp, "main");

			memcpy(onesim, similarity->d3[i], similarity->count[i]*sizeof(double));
			qsort_d_desc(onesim, 0, similarity->count[i]-1);
			for(j=0; j<similarity->count[i]; ++j) {
				fprintf(fp, "%ld, %.17f\n", j, onesim[j]);
			}

			fclose(fp);
		}
	}


	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
