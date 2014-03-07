/**
 * random select users from dataset to generate a new dataset.
 */

//#define NDEBUG  //for assert
#include <stdio.h>
#include <assert.h>
#include <time.h>
//#include <math.h>
#include <stdlib.h>
//#include <string.h>
#include "iilinefile.h"
#include "iidlinefile.h"
#include "bip.h"
#include "mt_random.h"
#include "error.h"
//#include "statistics.h"
//#include "hashtable.h"

void print_time(void) {
	time_t t=time(NULL); 
	printf("%s\n", ctime(&t)); 
	fflush(stdout);
}

void set_RandomSeed(void) {
	time_t t=time(NULL);
	unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);
}

int main(int argc, char **argv)
{
	print_time();

	char *netfilename;
	int usersnum;
	if (argc == 1) {
		netfilename = "data/rym/rym_2c";
		usersnum = 1000;
	}
	else if (argc == 3) {
		netfilename = argv[1];
		char *pEnd;
		usersnum = strtol(argv[2], &pEnd, 10);
	}
	else {
		isError("wrong argc argv.\n");
	}

	struct iiLineFile *netfile = create_iiLineFile(netfilename);
	struct Bipii *neti1 = create_Bipii(netfile, 1);
	free_iiLineFile(netfile);

	int *selectedusers = calloc(neti1->maxId + 1, sizeof(int));	
	assert(selectedusers != NULL);

	FILE *fp = fopen("newdataset", "w");
	
	int randomuser =  genrand_int31()%(neti1->maxId + 1);
	int num = 0;
	long i;
	if (usersnum > neti1->idNum) {
		isError("two big users' num, impossible.\n");
	}
	while(num<=usersnum && num <= neti1->idNum) {
		randomuser =  genrand_int31()%(neti1->maxId + 1);
		if (neti1->count[randomuser] && !selectedusers[randomuser]) {
			for (i=0; i<neti1->count[randomuser]; ++i) {
				fprintf(fp, "%d, %d\n", randomuser, neti1->id[randomuser][i]);
			}
			++num;
			selectedusers[randomuser] = 1;
		}
	}
	fclose(fp);
	free_Bipii(neti1);
	free(selectedusers);


	//struct iiLineFile *file = create_iiLineFile("newdataset");
	//wc_bip2_ii_HT(file, "subdataCon");
	//free_iiLineFile(file);

	print_time();
	return 0;
}
