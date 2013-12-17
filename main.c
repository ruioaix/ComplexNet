//#define NDEBUG  //for assert
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "inc/linefile/i3linefile.h"
#include "inc/compact/bip3i.h"
#include "inc/utility/random.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	char *netfilename;
	int loopNum;
	int maxscore;
	double theta, eta, epsilon, lambda;

	if (argc == 1) {
		netfilename = "data/movielen/movielens.txt";
		maxscore = 5;
		loopNum = 2;
		theta = 0.76; //score
		eta = 0.84; //degree
		epsilon = 0.78; //third
		lambda = 0.15; //hybrid
	}
	else if (argc == 8) {
		netfilename = argv[1];
		char *pEnd;
		maxscore = strtol(argv[2], &pEnd, 10);
		loopNum = strtol(argv[3], &pEnd, 10);
		theta = strtod(argv[4], &pEnd);
		eta = strtod(argv[5], &pEnd);
		epsilon = strtod(argv[6], &pEnd);
		lambda = strtod(argv[7], &pEnd);
	}
	else {
		printf("wrong argc\n");
		return 0;
	}

	//random number init, make sure that random number generated differently every time program run.
	unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);

	//main
	struct i3LineFile *netfile = create_i3LineFile(netfilename);
	struct Bip3i *neti1 = create_Bip3i(netfile, 1);
	struct Bip3i *neti2 = create_Bip3i(netfile, 0);
	free_i3LineFile(netfile);

	int i,j;
	double *score = calloc(neti2->maxId + 1, sizeof(double));
	assert(score != NULL);
	for (i=0; i<neti2->maxId + 1; ++i) {
		if (neti2->count[i]) {
			for (j=0; j<neti2->count[i]; ++j) {
				score[i] += neti2->i3[i][j];
			}
			score[i] /= neti2->count[i];
		}
	}

	double *rankA_s = calloc(neti2->maxId + 1, sizeof(double));
	double *rankA_d = calloc(neti2->maxId + 1, sizeof(double));
	double *rankA_t = calloc(neti2->maxId + 1, sizeof(double));
	double *rankA_h = calloc(neti2->maxId + 1, sizeof(double));
	assert(rankA_s != NULL);
	assert(rankA_d != NULL);
	assert(rankA_t != NULL);
	assert(rankA_h != NULL);

	struct L_Bip3i *smass_result = create_L_Bip3i();
	struct L_Bip3i *dmass_result = create_L_Bip3i();
	struct L_Bip3i *tmass_result = create_L_Bip3i();
	struct L_Bip3i *hybrid_result = create_L_Bip3i();

		double score_ave = 0;	
		double score_ave_2 = 0;
		double score_ave_3 = 0;
		double score_ave_4 = 0;
		for (i=0; i<loopNum; ++i) {

			struct i3LineFile *twofile = divide_Bip3i(neti1, neti2, 0.1);

			struct Bip3i *traini1 = create_Bip3i(twofile + 1, 1);
			struct Bip3i *traini2 = create_Bip3i(twofile + 1, 0);
			struct Bip3i *testi1 = create_Bip3i(twofile, 1);
			struct Bip3i *testi2 = create_Bip3i(twofile, 0);
			free_2_i3LineFile(twofile);

			struct iidLineFile *simfile = similarity_realtime_Bip3i(traini1, traini2);
			struct iidNet *trainSim = create_iidNet(simfile);
			free_iidLineFile(simfile);

			struct L_Bip3i *r1 = s_mass_Bip3i(traini1, traini2, testi1, testi2, trainSim, theta, maxscore);
			struct L_Bip3i *r2 = d_mass_Bip3i(traini1, traini2, testi1, testi2, trainSim, eta);
			struct L_Bip3i *r3 = thirdstepSD_mass_Bip3i(traini1, traini2, testi1, testi2, trainSim, epsilon);
			struct L_Bip3i *r4 = hybrid_Bip3i(traini1, traini2, testi1, testi2, trainSim, lambda);

			smass_result->R +=  r1->R;
			smass_result->PL += r1->PL;
			smass_result->HL += r1->HL;
			smass_result->IL += r1->IL;
			smass_result->NL += r1->NL;
			dmass_result->R +=  r2->R;
			dmass_result->PL += r2->PL;
			dmass_result->HL += r2->HL;
			dmass_result->IL += r2->IL;
			dmass_result->NL += r2->NL;
			tmass_result->R +=  r3->R;
			tmass_result->PL += r3->PL;
			tmass_result->HL += r3->HL;
			tmass_result->IL += r3->IL;
			tmass_result->NL += r3->NL;
			hybrid_result->R +=  r4->R;
			hybrid_result->PL += r4->PL;
			hybrid_result->HL += r4->HL;
			hybrid_result->IL += r4->IL;
			hybrid_result->NL += r4->NL;
			int L = r1->L;
			long len = L*(traini1->maxId + 1);
			for (j=0; j<L*(traini1->maxId + 1); ++j) {
				score_ave += score[r1->topL[j]]/len;
				score_ave_2 += score[r2->topL[j]]/len;
				score_ave_3 += score[r3->topL[j]]/len;
				score_ave_4 += score[r4->topL[j]]/len;
			}
			for (j=0; j<traini2->maxId + 1; ++j) {
				rankA_s[j] += r1->rankA[j];
				rankA_d[j] += r2->rankA[j];
				rankA_t[j] += r3->rankA[j];
				rankA_h[j] += r4->rankA[j];
			}
			
			free_Bip3i(traini1);
			free_Bip3i(traini2);
			free_Bip3i(testi1);
			free_Bip3i(testi2);
			free_iidNet(trainSim);
			free_L_Bip3i(r1);
			free_L_Bip3i(r2);
			free_L_Bip3i(r3);
			free_L_Bip3i(r4);
		}
		printf("score_mass\ttheta: %f, loopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f, Score: %f\n", theta, loopNum, smass_result->R/loopNum, smass_result->PL/loopNum, smass_result->IL/loopNum, smass_result->HL/loopNum, smass_result->NL/loopNum, score_ave/loopNum);
		printf("degree_mass\teta: %f, loopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f, Score: %f\n", eta, loopNum, dmass_result->R/loopNum, dmass_result->PL/loopNum, dmass_result->IL/loopNum, dmass_result->HL/loopNum, dmass_result->NL/loopNum, score_ave_2/loopNum);
		printf("third_mass\tepsilon: %f, loopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f, Score: %f\n", epsilon, loopNum, tmass_result->R/loopNum, tmass_result->PL/loopNum, tmass_result->IL/loopNum, tmass_result->HL/loopNum, tmass_result->NL/loopNum, score_ave_3/loopNum);
		printf("hybrid\tlambda: %f, loopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f, Score: %f\n", lambda, loopNum, hybrid_result->R/loopNum, hybrid_result->PL/loopNum, hybrid_result->IL/loopNum, hybrid_result->HL/loopNum, hybrid_result->NL/loopNum, score_ave_3/loopNum);

		FILE *fp = fopen("score_mass_rankrating", "w");
		FILE *fp1 = fopen("degree_mass_rankrating", "w");
		FILE *fp2 = fopen("third_mass_rankrating", "w");
		FILE *fp3 = fopen("hybrid_rankrating", "w");
		for (i=0; i<neti1->maxId + 1; ++i) {
			if (neti1->count[i]) {
				fprintf(fp, "%d, rank: %.17f, rating: %.17f\n", i, rankA_s[i], score[i]);
				fprintf(fp1, "%d, rank: %.17f, rating: %.17f\n", i, rankA_d[i], score[i]);
				fprintf(fp2, "%d, rank: %.17f, rating: %.17f\n", i, rankA_t[i], score[i]);
				fprintf(fp3, "%d, rank: %.17f, rating: %.17f\n", i, rankA_h[i], score[i]);
			}
		}

	free_L_Bip3i(smass_result);
	free_L_Bip3i(dmass_result);
	free_L_Bip3i(tmass_result);
	free_L_Bip3i(hybrid_result);
	free(score);
	free_Bip3i(neti1);
	free_Bip3i(neti2);

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
