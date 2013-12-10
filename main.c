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
	double begin;
	if (argc == 1) {
		netfilename = "data/movielen/movielens.txt";
		loopNum = 3;
		begin = 0;
	}
	else if (argc == 4) {
		netfilename = argv[1];
		char *pEnd;
		loopNum = strtol(argv[2], &pEnd, 10);
		begin = strtod(argv[3], &pEnd);
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

	int k,i,j;
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

	struct L_Bip3i *smass_result = create_L_Bip3i();
	struct L_Bip3i *dmass_result = create_L_Bip3i();
	struct L_Bip3i *tmass_result = create_L_Bip3i();

	//struct i3LineFile *trainfile = create_i3LineFile(trainfilename);
	//struct i3LineFile *testfile = create_i3LineFile(testfilename);

	for (k=0; k<10; ++k) {
		double theta = begin + k*0.02;
		clean_L_Bip3i(smass_result);
		clean_L_Bip3i(dmass_result);
		clean_L_Bip3i(tmass_result);
		double score_ave = 0;	
		double score_ave_2 = 0;
		double score_ave_3 = 0;
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

			struct L_Bip3i *r1 = s_mass_Bip3i(traini1, traini2, testi1, testi2, trainSim, theta);
			struct L_Bip3i *r2 = d_mass_Bip3i(traini1, traini2, testi1, testi2, trainSim, theta);
			struct L_Bip3i *r3 = thirdstepSD_mass_Bip3i(traini1, traini2, testi1, testi2, trainSim, theta);

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
			int L = r1->L;
			long len = L*(traini1->maxId + 1);
			for (j=0; j<L*(traini1->maxId + 1); ++j) {
				score_ave += score[r1->topL[j]]/len;
				score_ave_2 += score[r2->topL[j]]/len;
				score_ave_3 += score[r3->topL[j]]/len;
			}
			
			free_Bip3i(traini1);
			free_Bip3i(traini2);
			free_Bip3i(testi1);
			free_Bip3i(testi2);
			free_iidNet(trainSim);
			free_L_Bip3i(r1);
			free_L_Bip3i(r2);
			free_L_Bip3i(r3);
		}
		printf("score_mass\ttheta: %f, loopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f, Score: %f\n", theta, loopNum, smass_result->R/loopNum, smass_result->PL/loopNum, smass_result->IL/loopNum, smass_result->HL/loopNum, smass_result->NL/loopNum, score_ave/loopNum);
		printf("degree_mass\teta: %f, loopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f, Score: %f\n", theta, loopNum, dmass_result->R/loopNum, dmass_result->PL/loopNum, dmass_result->IL/loopNum, dmass_result->HL/loopNum, dmass_result->NL/loopNum, score_ave_2/loopNum);
		printf("third_mass\teta: %f, loopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f, Score: %f\n", theta, loopNum, tmass_result->R/loopNum, tmass_result->PL/loopNum, tmass_result->IL/loopNum, tmass_result->HL/loopNum, tmass_result->NL/loopNum, score_ave_3/loopNum);
	}

	free_L_Bip3i(smass_result);
	free_L_Bip3i(dmass_result);
	free_L_Bip3i(tmass_result);
	free(score);
	free_Bip3i(neti1);
	free_Bip3i(neti2);

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
