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

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	char *netfilename;
	double simcut;
	if (argc == 1) {
		netfilename = "data/movielens/movielens_2c";
		simcut = 0.1;	
	}
	else if (argc == 3) {
		netfilename = argv[1];
		char *p;
		simcut = strtod(argv[2], &p);
	}

	//unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	//init_by_array(init, length);

	struct iiLineFile *netfile = create_iiLineFile(netfilename);
	struct Bip2 *neti1 = create_Bip2(netfile, 1);
	struct Bip2 *neti2 = create_Bip2(netfile, 0);
	free_iiLineFile(netfile);
	struct iiLineFile *twofile = divide_Bip2(neti1, neti2, 0.1);
	free_Bip2(neti1);
	free_Bip2(neti2);
	struct Bip2 *traini1 = create_Bip2(twofile + 1, 1);
	struct Bip2 *traini2 = create_Bip2(twofile + 1, 0);
	struct Bip2 *testi1 = create_Bip2(twofile, 1);
	struct Bip2 *testi2 = create_Bip2(twofile, 0);
	free_2_iiLineFile(twofile);
	
	//to this, only traini1/2, testi1/2 available.

	struct iidLineFile *userSimilarityfile = similarity_Bip2(traini1, traini2, 1);
	struct iidNet *userSim = create_iidNet(userSimilarityfile);
	sort_desc_iidNet(userSim);
	free_iidLineFile(userSimilarityfile);

	struct iidLineFile *itemSimilarityfile = similarity_Bip2(traini1, traini2, 0);
	struct iidNet *itemSim = create_iidNet(itemSimilarityfile);
	free_iidLineFile(itemSimilarityfile);
	
	//to this, only traini1/2, testi1/2 ,user/itemSim, available.
	
	//experiment_knn_Bip2(traini1, traini2, testi1, testi2, userSim);
	//struct L_Bip *mass_result = probs_Bip2(traini1, traini2, testi1, testi2, itemSim);
	//struct L_Bip *simcut_result = probs_simcut_Bip2(traini1, traini2, testi1, testi2, itemSim, userSim, simcut);
	int i;
	for(i=0; i<traini1->maxId + 1; ++i) {
		struct L_Bip *simcut_result = topR_probs_Bip2(traini1, traini2, testi1, testi2, itemSim, userSim, i);
		printf("simcut\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f, simcut: %f\n", simcut_result->R, simcut_result->PL, simcut_result->IL, simcut_result->HL, simcut_result->NL, simcut);
		free_L_Bip(simcut_result);
	}

	//printf("mass\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", mass_result->R, mass_result->PL, mass_result->IL, mass_result->HL, mass_result->NL);

	free_iidNet(userSim);
	free_iidNet(itemSim);
	free_Bip2(traini1);
	free_Bip2(traini2);
	free_Bip2(testi1);
	free_Bip2(testi2);
	//free_L_Bip(mass_result);
	//free_L_Bip(simcut_result);
	

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
