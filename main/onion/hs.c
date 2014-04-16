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
#include "base.h"
#include "linefile.h"
#include "bip.h"
#include "mtprand.h"

void create_2dataset(char *netfilename, struct Bip **traini1, struct Bip **traini2, struct Bip **testi1, struct Bip **testi2, double rate) {
	struct LineFile *netfile = create_LineFile(netfilename, 1, 1, -1);
	struct Bip *neti1 = create_Bip(netfile, 1);
	struct Bip *neti2 = create_Bip(netfile, 2);
	free_LineFile(netfile);
	struct LineFile *first, *second;
	divide_Bip(neti1, neti2, rate, &first, &second);
	free_Bip(neti1);
	free_Bip(neti2);
	*traini1 = create_Bip(second, 1);
	*traini2 = create_Bip(second, 2);
	*testi1 = create_Bip(first, 1);
	*testi2 = create_Bip(first, 2);
	free_LineFile(first);
	free_LineFile(second);
}

void get_UserSimilarity(struct Bip *traini1, struct Bip *traini2, struct iidNet **userSim) {
	struct LineFile *userSimilarityfile = similarity_Bip(traini1, traini2, 1);
	//struct LineFile *userSimilarityfile = mass_similarity_Bip(traini1, traini2);
	*userSim = create_iidNet(userSimilarityfile);
	free_LineFile(userSimilarityfile);
}

void get_ItemSimilarity(struct Bip *traini1, struct Bip *traini2, struct iidNet **itemSim) {
	struct LineFile *itemSimilarityfile = similarity_Bip(traini1, traini2, 2);
	*itemSim = create_iidNet(itemSimilarityfile);
	free_LineFile(itemSimilarityfile);
}

void test_ArgcArgv(int argc, char **argv, char **netfilename, double *rate) {
	if (argc == 1) {
		*netfilename = "../movielens/movielens_943_1574_82520";
		*rate = 0.1;
	}
	else if (argc == 3) {
		*netfilename = argv[1];
		char *p;
		*rate = strtod(argv[2], &p);
	}
	else {
		isError("wrong argc, argv.\n");
	}
}

int main(int argc, char **argv)
{
	print_time();
	//set_timeseed_MTPR();

	double rate;
	char *netfilename;
	test_ArgcArgv(argc, argv, &netfilename, &rate); 

	struct Bip *traini1, *traini2, *testi1, *testi2;
	create_2dataset(netfilename, &traini1, &traini2, &testi1, &testi2, rate);

	struct iidNet *userSim, *itemSim=NULL;
	get_UserSimilarity(traini1, traini2, &userSim);
	get_ItemSimilarity(traini1, traini2, &itemSim);
	
	sort_desc_iidNet(userSim);

	int i;
	int max_similaruer = 0;
	for (i=0; i<userSim->maxId + 1; ++i) {
		max_similaruer = max_similaruer>userSim->count[i]?max_similaruer:userSim->count[i];
	}
	printf("topk stop at =>> %d\n", max_similaruer /= 2);

	for (i = 0; i < max_similaruer; ++i) {
		struct Metrics_Bip *hs_result = mass_hs_Bip(traini1, traini2, testi1, testi2, userSim, itemSim, i);
		printf("hs result =>> Rate: %f, topk: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", rate, i, hs_result->R, hs_result->PL, hs_result->IL, hs_result->HL, hs_result->NL);
		free_MetricsBip(hs_result);
	}

	free_Bip(traini1);
	free_Bip(traini2);
	free_Bip(testi1);
	free_Bip(testi2);
	free_iidNet(userSim);
	free_iidNet(itemSim);
	
	print_time();
	return 0;
}
