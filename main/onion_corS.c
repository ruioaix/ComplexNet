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
#include "iilinefile.h"
#include "iidlinefile.h"
#include "bip.h"
#include "error.h"
#include "mt_random.h"

void set_RandomSeed(void) {
	time_t t=time(NULL);
	unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);
}

void print_time(void) {
	time_t t=time(NULL); 
	printf("%s\n", ctime(&t)); 
	fflush(stdout);
}

void create_2dataset(char *netfilename, struct Bipii **traini1, struct Bipii **traini2, struct Bipii **testi1, struct Bipii **testi2) {
	struct iiLineFile *netfile = create_iiLineFile(netfilename);
	struct Bipii *neti1 = create_Bipii(netfile, 1);
	struct Bipii *neti2 = create_Bipii(netfile, 0);
	free_iiLineFile(netfile);
	struct iiLineFile *first, *second;
	divide_Bipii(neti1, neti2, 0.1, &first, &second);
	free_Bipii(neti1);
	free_Bipii(neti2);
	*traini1 = create_Bipii(second, 1);
	*traini2 = create_Bipii(second, 0);
	*testi1 = create_Bipii(first, 1);
	*testi2 = create_Bipii(first, 0);
	free_iiLineFile(first);
	free_iiLineFile(second);
}

void get_UserSimilarity(struct Bipii *traini1, struct Bipii *traini2, struct iidNet **userSim) {
	struct iidLineFile *userSimilarityfile = similarity_Bipii(traini1, traini2, 1);
	*userSim = create_iidNet(userSimilarityfile);
	free_iidLineFile(userSimilarityfile);
}

void get_ItemSimilarity(struct Bipii *traini1, struct Bipii *traini2, struct iidNet **itemSim) {
	struct iidLineFile *itemSimilarityfile = similarity_Bipii(traini1, traini2, 0);
	*itemSim = create_iidNet(itemSimilarityfile);
	free_iidLineFile(itemSimilarityfile);
}

void test_ArgcArgv(int argc, char **argv, char **netfilename) {
	if (argc == 1) {
		*netfilename = "data/delicious/delicious_2c_sub1000";
		//*netfilename = "data/movielens/movielens_2c";
		//*colK = 0.2;	
	}
	else if (argc == 2) {
		*netfilename = argv[1];
		//char *p;
		//*colK = strtod(argv[2], &p);
	}
	else {
		isError("wrong argc, argv.\n");
	}
}

int main(int argc, char **argv)
{
	print_time();
	//set_RandomSeed();

	char *netfilename;
	test_ArgcArgv(argc, argv, &netfilename); 

	struct Bipii *traini1, *traini2, *testi1, *testi2;
	create_2dataset(netfilename, &traini1, &traini2, &testi1, &testi2);

	struct iidNet *userSim, *itemSim=NULL;
	get_UserSimilarity(traini1, traini2, &userSim);
	get_ItemSimilarity(traini1, traini2, &itemSim);
	
	sort_desc_iidNet(userSim);

	struct Metrics_Bipii *mass_result = mass_Bipii(traini1, traini2, testi1, testi2, itemSim);
	printf("mass\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", mass_result->R, mass_result->PL, mass_result->IL, mass_result->HL, mass_result->NL);
	free_MetricsBipii(mass_result);

	int i;
	for(i=0; i<101; ++i) {
		double corS = i*0.01;
		struct Metrics_Bipii *corS_result = mass_corS_Bipii(traini1, traini2, testi1, testi2, itemSim, userSim, corS);
		printf("corS\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f, corS: %f\n", corS_result->R, corS_result->PL, corS_result->IL, corS_result->HL, corS_result->NL, corS);fflush(stdout);
		free_MetricsBipii(corS_result);
	}
	

	free_Bipii(traini1);
	free_Bipii(traini2);
	free_Bipii(testi1);
	free_Bipii(testi2);
	free_iidNet(userSim);
	free_iidNet(itemSim);
	//free_L_Bip(mass_result);
	//free_L_Bip(simcut_result);
	
	print_time();
	return 0;
}
