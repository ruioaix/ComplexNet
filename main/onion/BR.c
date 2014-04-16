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

void create_2dataset(char *netfilename, struct iiBip **traini1, struct iiBip **traini2, struct iiBip **testi1, struct iiBip **testi2) {
	struct LineFile *netfile = create_LineFile(netfilename, 1, 1, -1);
	struct iiBip *neti1 = create_iiBip(netfile, 1);
	struct iiBip *neti2 = create_iiBip(netfile, 2);
	free_LineFile(netfile);
	struct LineFile *first, *second;
	divide_iiBip(neti1, neti2, 0.1, &first, &second);
	free_iiBip(neti1);
	free_iiBip(neti2);
	*traini1 = create_iiBip(second, 1);
	*traini2 = create_iiBip(second, 2);
	*testi1 = create_iiBip(first, 1);
	*testi2 = create_iiBip(first, 2);
	free_LineFile(first);
	free_LineFile(second);
}

void get_UserSimilarity(struct iiBip *traini1, struct iiBip *traini2, struct iidNet **userSim) {
	struct LineFile *userSimilarityfile = similarity_iiBip(traini1, traini2, 1);
	//struct LineFile *userSimilarityfile = mass_similarity_iiBip(traini1, traini2);
	*userSim = create_iidNet(userSimilarityfile);
	free_LineFile(userSimilarityfile);
}

void get_ItemSimilarity(struct iiBip *traini1, struct iiBip *traini2, struct iidNet **itemSim) {
	struct LineFile *itemSimilarityfile = similarity_iiBip(traini1, traini2, 2);
	*itemSim = create_iidNet(itemSimilarityfile);
	free_LineFile(itemSimilarityfile);
}

void test_ArgcArgv(int argc, char **argv, char **netfilename, int *begin, int *end) {
	if (argc == 1) {
		*netfilename = "netflix_2c";
		*begin = 3;
		*end = 5;
	}
	else if (argc == 4) {
		*netfilename = argv[1];
		char *p;
		*begin = strtod(argv[2], &p);
		*end = strtod(argv[3], &p);
	}
	else {
		isError("wrong argc, argv.\n");
	}
}

int main(int argc, char **argv)
{
	print_time();
	//set_timeseed_MTPR();

	int begin, end;
	char *netfilename;
	test_ArgcArgv(argc, argv, &netfilename, &begin, &end); 

	struct iiBip *traini1, *traini2, *testi1, *testi2;
	create_2dataset(netfilename, &traini1, &traini2, &testi1, &testi2);

	struct iidNet *userSim, *itemSim=NULL;
	get_UserSimilarity(traini1, traini2, &userSim);
	get_ItemSimilarity(traini1, traini2, &itemSim);
	
	sort_desc_iidNet(userSim);

	mass_getBK_iiBip(traini1, traini2, testi1, testi2, userSim, begin, end);

	free_iiBip(traini1);
	free_iiBip(traini2);
	free_iiBip(testi1);
	free_iiBip(testi2);
	free_iidNet(userSim);
	free_iidNet(itemSim);
	
	print_time();
	return 0;
}
