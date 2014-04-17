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

	struct Metrics_Bip *score_result = create_MetricsBip();
	struct Metrics_Bip *s3stp_result = create_MetricsBip();
	struct Metrics_Bip *degre_result = create_MetricsBip();

	clean_MetricsBip(score_result);
	clean_MetricsBip(s3stp_result);
	clean_MetricsBip(degre_result);
	double aveScore1 = 0;
	double aveScore2 = 0;
	double aveScore3 = 0;

	for (j=0; j<loopNum; ++j) {

		struct LineFile *first, *second;
		divide_Bip(neti1, neti2, 0.1, &first, &second);
		struct Bip *traini1 = create_Bip(second, 1);
		struct Bip *traini2 = create_Bip(second, 2);
		struct Bip *testi1 = create_Bip(first, 1);
		struct Bip *testi2 = create_Bip(first, 2);
		free_LineFile(first);
		free_LineFile(second);

		struct LineFile *simfile = similarity_Bip(traini1, traini2, 2);
		struct iidNet *itemSim = create_iidNet(simfile);
		free_LineFile(simfile);

		struct Metrics_Bip *r1 = mass_score_Bip(traini1, traini2, testi1, testi2, itemSim, maxscore, theta);
		struct Metrics_Bip *r2 = mass_scoret3step_Bip(traini1, traini2, testi1, testi2, itemSim, theta);
		struct Metrics_Bip *r3 = mass_degree_Bip(traini1, traini2, testi1, testi2, itemSim, theta);

		score_result->R +=  r1->R;
		score_result->PL += r1->PL;
		score_result->HL += r1->HL;
		score_result->IL += r1->IL;
		score_result->NL += r1->NL;
		s3stp_result->R +=  r2->R;
		s3stp_result->PL += r2->PL;
		s3stp_result->HL += r2->HL;
		s3stp_result->IL += r2->IL;
		s3stp_result->NL += r2->NL;
		degre_result->R +=  r3->R;
		degre_result->PL += r3->PL;
		degre_result->HL += r3->HL;
		degre_result->IL += r3->IL;
		degre_result->NL += r3->NL;

		int L = r1->L;
		long len = L*(traini1->maxId + 1);
		for (i=0; i<L*(traini1->maxId + 1); ++i) {
			aveScore1 += score[r1->topL[i]]/len;
			aveScore2 += score[r2->topL[i]]/len;
			aveScore3 += score[r3->topL[i]]/len;
		}
		
		free_Bip(traini1);
		free_Bip(traini2);
		free_Bip(testi1);
		free_Bip(testi2);
		free_iidNet(itemSim);
		free_MetricsBip(r1);
		free_MetricsBip(r2);
		free_MetricsBip(r3);
	}

	printf("score result =>> lN: %d, theta: %f, R: %f, PL: %f, IL: %f, HL: %f, NL: %f, SL: %f\n", loopNum, theta, score_result->R/loopNum, score_result->PL/loopNum, score_result->IL/loopNum, score_result->HL/loopNum, score_result->NL/loopNum, aveScore1/loopNum);
	printf("s3stp result =>> lN: %d, theta: %f, R: %f, PL: %f, IL: %f, HL: %f, NL: %f, SL: %f\n", loopNum, theta, s3stp_result->R/loopNum, s3stp_result->PL/loopNum, s3stp_result->IL/loopNum, s3stp_result->HL/loopNum, s3stp_result->NL/loopNum, aveScore2/loopNum);
	printf("degre result =>> lN: %d, theta: %f, R: %f, PL: %f, IL: %f, HL: %f, NL: %f, SL: %f\n", loopNum, theta, degre_result->R/loopNum, degre_result->PL/loopNum, degre_result->IL/loopNum, degre_result->HL/loopNum, degre_result->NL/loopNum, aveScore3/loopNum);


	free_MetricsBip(score_result);
	free_MetricsBip(s3stp_result);
	free_MetricsBip(degre_result);
	free_Bip(neti1);
	free_Bip(neti2);
	free(score);
	print_time();
	return 0;
}
