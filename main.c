/** 
 * ./run 
 * ./run data/netflix/netflix_3c 3
 * ./run data/netflix/netflix_2c 2
 * ./run data/netflix/netflix_3c 4 3
 * ./run data/rym/rym_3c 4 6
 *
 */

//#define NDEBUG  //for assert
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "inc/linefile/iilinefile.h"
#include "inc/linefile/i3linefile.h"
#include "inc/compact/bip2.h"
#include "inc/compact/bip3i.h"
#include "inc/utility/error.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	char *netfilename;
	int cNum, passScore;
	if (argc == 1) {
		netfilename = "data/movielens/movielens_2c";
		cNum = 2;
	}
	else if (argc == 3) {
		netfilename = argv[1];
		char *p;
		cNum = strtol(argv[2], &p, 10);
	}
	else if (argc == 4) {
		netfilename = argv[1];
		char *p;
		cNum = strtol(argv[2], &p, 10);
		passScore = strtol(argv[3], &p, 10);	
	}

	if (cNum == 2) {
		struct iiLineFile *c2file = create_iiLineFile(netfilename);
		struct Bip2 *bipi1 = create_Bip2(c2file, 1);
		struct Bip2 *bipi2 = create_Bip2(c2file, 0);
		verifyBip2(bipi1, bipi2);
		free_iiLineFile(c2file);
		free_Bip2(bipi1);
		free_Bip2(bipi2);
	}

	if (cNum == 3) {
		struct i3LineFile *c3file = create_i3LineFile(netfilename);
		struct Bip3i *bipi1 = create_Bip3i(c3file, 1);
		struct Bip3i *bipi2 = create_Bip3i(c3file, 0);
		verifyBip3i(bipi1, bipi2);
		free_i3LineFile(c3file);
		free_Bip3i(bipi1);
		free_Bip3i(bipi2);
	}

	if (cNum == 4) {
		struct i3LineFile *c3file = create_i3LineFile(netfilename);
		FILE *fp = fopen("NewC2file", "w");
		fileError(fp, "main cNum = 4");
		long i;
		for (i=0; i<c3file->linesNum; ++i) {
			if (c3file->lines[i].i3 >= passScore) {
				fprintf(fp, "%d, %d\n", c3file->lines[i].i1, c3file->lines[i].i2);
			}
		}
		fclose(fp);
		free_i3LineFile(c3file);
	}

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
