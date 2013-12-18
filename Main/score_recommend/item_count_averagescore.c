//#define NDEBUG  //for assert
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "inc/linefile/i3linefile.h"
#include "inc/compact/bip3i.h"
#include "inc/linefile/iilinefile.h"
//#include "inc/linefile/iidlinefile.h"
#include "inc/compact/bip2.h"
//#include "inc/compact/iidnet.h"
#include "inc/utility/error.h"
#include "inc/utility/random.h"
//#include "inc/utility/hashtable.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	char *netfilename;
	if (argc == 1) {
		netfilename = "data/movielen/movielens.txt";
	}
	if (argc == 2) {
		netfilename = argv[1];
	}

	//printf("%ld\n", t);
	unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);

	struct i3LineFile *file = create_i3LineFile(netfilename);
	//struct Bip3i *bipi1 = create_Bip3i(file, 1);
	struct Bip3i *bipi2 = create_Bip3i(file, 0);

	FILE *fp = fopen("Results/sandiantu", "w");
	fileError(fp, "main");
	int i;
	long j;
	for (i=0; i<bipi2->maxId + 1; ++i) {
		if (bipi2->count[i]) {
			int _r = 0;
			for (j=0; j<bipi2->count[i]; ++j) {
				_r += bipi2->i3[i][j];
			}
			fprintf(fp, "%ld, %d, %.17f\n", bipi2->count[i], i, (double)_r/(double)bipi2->count[i]);
		}
	}
	fclose(fp);

	double *countlist = calloc((bipi2->edgesNum),sizeof(double));
	assert(countlist != NULL);
	int *countlistNum = calloc((bipi2->edgesNum),sizeof(int));
	assert(countlistNum != NULL);
	for (i=0; i<bipi2->maxId + 1; ++i) {
		if (bipi2->count[i]) {
			int _r = 0;
			for (j=0; j<bipi2->count[i]; ++j) {
				_r += bipi2->i3[i][j];
			}
			countlist[bipi2->count[i]] += (double)_r/(double)bipi2->count[i];
			++countlistNum[bipi2->count[i]];
		}
	}
	fp = fopen("Results/sandiantu_ave", "w");
	for (j=0; j<bipi2->edgesNum; ++j) {
		if (countlistNum[j]) {
			fprintf(fp, "%ld, %.17f\n", j, countlist[j]/countlistNum[j]);
		}
	}
	fclose(fp);

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
