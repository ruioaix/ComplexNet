/**
 * onion model knn implementation
 *
 * 1, read a biparte net, and divide it into two parts: trainset,testset.
 * 2, for each user in testset, first I use the first two step of mass diffusion, then 
 *    I will use the user-similarity to decide how many similar users I will use on the third step.
 *    actually, for one user, I have to do many calculations.
 *    for example, if one user has 100 similar users, I will need to calculate 100 times.
 *    	the first time: I use the top 1 user to calculate.
 *    	the second time: I use the top 2 users to calculate.
 *    	the third time: I use the top 3 users to calculate.
 *    	...
 *    	the 100 time: I use all 100 users.
 *    and this is only one user.
 *    the result is that: one user to one simliar users' number.
 * 3, use the new data to diffusion, get rankscore and all other things. and use mass diffusion too, get compared.
 *
 */

//#define NDEBUG  //for assert
#include <stdio.h>
#include <assert.h>
#include <time.h>
//#include <math.h>
#include <stdlib.h>
//#include <string.h>
#include "inc/linefile/iilinefile.h"
#include "inc/linefile/iidlinefile.h"
#include "inc/compact/bip2.h"
//#include "inc/utility/error.h"
#include "inc/utility/random.h"
#include "inc/utility/statistics.h"
#include "inc/utility/hashtable.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	char *netfilename;
	if (argc == 1) {
		netfilename = "data/movielens/movielens_2c";
	}
	else if (argc == 2) {
		netfilename = argv[1];
	}

	//unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	//init_by_array(init, length);

	struct iiLineFile *netfile = create_iiLineFile(netfilename);
	struct Bip2 *neti1 = create_Bip2(netfile, 1);
	//struct Bip2 *neti2 = create_Bip2(netfile, 0);
	//free_iiLineFile(netfile);

	int *selectedusers = calloc(neti1->maxId + 1, sizeof(int));	
	assert(selectedusers != NULL);

	FILE *fp = fopen("subdata", "w");
	
	int randomuser =  genrand_int31()%(neti1->maxId + 1);
	int num = 0;
	long i;
	while(num<=1000 && num <= neti1->idNum) {
		randomuser =  genrand_int31()%(neti1->maxId + 1);
		if (!selectedusers[randomuser]) {
			for (i=0; i<neti1->count[randomuser]; ++i) {
				fprintf(fp, "%d, %d\n", randomuser, neti1->id[randomuser][i]);
			}
			++num;
			selectedusers[randomuser] = 1;
		}
	}
	fclose(fp);


	struct iiLineFile *file = create_iiLineFile("subdata");
	wc_bip2_ii_HT(file, "subdataCon");

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
