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
#include <math.h>
#include <stdlib.h>
//#include <string.h>
#include "base.h"
#include "linefile.h"
#include "bip.h"
#include "mtprand.h"

void create_2dataset(char *netfilename, struct Bip **traini1, struct Bip **traini2, struct Bip **testi1, struct Bip **testi2) {
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

void test_ArgcArgv(int argc, char **argv, char **netfilename, int *loopNum, int *maxscore, double *theta) {
	if (argc == 1) {
		*netfilename = "repo/stage1/data/movielens/movielens_3c";
		*loopNum = 2;
		*maxscore = 5;
		*theta = 0;
	}
	else if (argc == 5) {
		*netfilename = argv[1];
		char *p;
		*loopNum = strtol(argv[2], &p, 10);
		*maxscore = strtol(argv[3], &p, 10);
		*theta = strtod(argv[4], &p);
	}
	else {
		isError("wrong argc, argv.\n");
	}
}

int main(int argc, char **argv)
{
	print_time();
	//set_timeseed_MTPR();

	char *netfilename;
	int loopNum, maxscore;
	double theta; 
	test_ArgcArgv(argc, argv, &netfilename, &loopNum, &maxscore, &theta); 

	struct LineFile *netfile = create_LineFile(netfilename, 1, 1, 1, -1);
	struct Bip *neti1 = create_Bip(netfile, 1);
	struct Bip *neti2 = create_Bip(netfile, 2);
	free_LineFile(netfile);

	int i,j;
	double *score = calloc(neti2->maxId + 1, sizeof(double));
	assert(score != NULL);
	for (i=0; i<neti2->maxId + 1; ++i) {
		if (neti2->count[i]) {
			for (j=0; j<neti2->count[i]; ++j) {
				score[i] += neti2->score[i][j];
			}
			score[i] /= neti2->count[i];
		}
	}

	double *r1 = mass_score_rank_Bip(neti1, neti2, maxscore, theta);
	double *r2 = mass_scoret3step_rank_Bip(neti1, neti2, theta);
	double *r3 = mass_degree_rank_Bip(neti1, neti2, theta);

	int stepNum = 5;
	double *s_rankA	= calloc(stepNum, sizeof(double));
	double *d_rankA	= calloc(stepNum, sizeof(double));
	double *t_rankA	= calloc(stepNum, sizeof(double));
	double *h_rankA	= calloc(stepNum, sizeof(double));
	double *m_rankA	= calloc(stepNum, sizeof(double));
	assert(s_rankA != NULL && d_rankA != NULL && t_rankA != NULL && h_rankA != NULL && m_rankA != NULL);

	int *s = calloc(stepNum, sizeof(int));
	for (i=0; i<neti2->maxId + 1; ++i) {
		if (neti2->count[i]) {
			//int r = 1;
			int r = floor((stepNum-1)*score[i]/5);
			++s[r];
			s_rankA[r] += r1[i];
			d_rankA[r] += r2[i];
			t_rankA[r] += r3[i];
			//h_rankA[r] += rankA_h[i];
			//m_rankA[r] += rankA_m[i];
		}
	}

	for (i=0; i<stepNum; ++i) {
		if (s[i]) {
			printf("%d, %.17f, %.17f, %.17f, %.17f\n", i, 5.0*((double)i)/stepNum, s_rankA[i]/s[i], d_rankA[i]/s[i], t_rankA[i]/s[i]);
		}
	}
	
	free(r1);
	free(r2);
	free(r3);

	free_Bip(neti1);
	free_Bip(neti2);
	free(score);
	print_time();
	return 0;
}
