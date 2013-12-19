//#define NDEBUG  //for assert
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "inc/linefile/i5linefile.h"
#include "inc/linefile/iilinefile.h"
#include "inc/linefile/i3linefile.h"
#include "inc/linefile/iidlinefile.h"
#include "inc/compact/bip2.h"
#include "inc/compact/bip3i.h"
#include "inc/compact/iidnet.h"
#include "inc/utility/error.h"
#include "inc/utility/random.h"
#include "inc/utility/hashtable.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	char *netfilename;
	int loopNum;
	double stepLen;
	int stepNum;
	double stepbegin;
	if (argc == 1) {
		netfilename = "data/movielen/movielens.txt";
		loopNum = 1;
		stepbegin = 0;
		stepLen = 0.01;
		stepNum = 10;
	}
	else if (argc == 6) {
		netfilename = argv[1];
		char *pEnd;
		loopNum = strtol(argv[2], &pEnd, 10);
		stepbegin = strtod(argv[3], &pEnd);
		stepLen = strtod(argv[4], &pEnd);
		stepNum = strtol(argv[5], &pEnd, 10);
	}
	else {
		printf("wrong argc\n");
		return 0;
	}

	//printf("%ld\n", t);
	unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);

	struct i3LineFile *scorefile = create_i3LineFile(netfilename);
	struct Bip3i *scorei1 = create_Bip3i(scorefile, 1);
	struct Bip3i *scorei2 = create_Bip3i(scorefile, 0);
	free_i3LineFile(scorefile);
	int i;
	long j;
	double *score = calloc(scorei2->maxId + 1, sizeof(double));
	assert(score != NULL);
	for (i=0; i<scorei2->maxId + 1; ++i) {
		if (scorei2->count[i]) {
			for (j=0; j<scorei2->count[i]; ++j) {
				score[i] += scorei2->i3[i][j];
			}
			score[i] /= scorei2->count[i];
		}
	}
	free_Bip3i(scorei1);
	free_Bip3i(scorei2);


	//divide file into two part: train and test.
	//train will contain every user and every item.
	//but test maybe not.
	struct iiLineFile *net_file = create_iiLineFile(netfilename);
	struct Bip2 *seti1 = create_Bip2(net_file, 1);
	struct Bip2 *seti2 = create_Bip2(net_file, 0);
	free_iiLineFile(net_file);

	struct L_Bip2 *hybrid_result = create_L_Bip2(); 

	int k;
	double lambda;
	for (k=0; k<stepNum; ++k) {
		lambda = k*stepLen + stepbegin;
		clean_L_Bip2(hybrid_result);
		double score_ave = 0;
		for (i=0; i<loopNum; ++i) {
			struct iiLineFile *n2file = divide_Bip2(seti1, seti2, 0.1);

			struct Bip2 *traini1= create_Bip2(n2file + 1, 1);
			struct Bip2 *traini2 = create_Bip2(n2file + 1, 0);
			struct Bip2 *testi1 = create_Bip2(n2file, 1);
			struct Bip2 *testi2 = create_Bip2(n2file, 0);
			free_2_iiLineFile(n2file);

			//the similarity is get from traini1
			//struct iidLineFile *similarity = create_iidLineFile(simfilename);
			struct iidLineFile *similarity = similarity_realtime_Bip2(traini1, traini2);
			struct iidNet *trainSim = create_iidNet(similarity);
			free_iidLineFile(similarity);

			struct L_Bip2 *r4 = hybrid_Bip2(traini1, traini2, testi1, testi2, trainSim, lambda);

			hybrid_result->R +=  r4->R;
			hybrid_result->PL += r4->PL;
			hybrid_result->HL += r4->HL;
			hybrid_result->IL += r4->IL;
			hybrid_result->NL += r4->NL;
			int L = r4->L;
			int *topL = r4->topL;
			long len = L*(traini1->maxId + 1);
			for (j=0; j<L*(traini1->maxId + 1); ++j) {
				score_ave += score[topL[j]]/len;
			}

			free_iidNet(trainSim);
			free_Bip2(traini1);
			free_Bip2(traini2);
			free_Bip2(testi1);
			free_Bip2(testi2);
			free_L_Bip2(r4);
		}
		printf("      hybrid\tlambda: %f, loopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f, Score: %f\n", lambda, loopNum, hybrid_result->R/loopNum, hybrid_result->PL/loopNum, hybrid_result->IL/loopNum, hybrid_result->HL/loopNum, hybrid_result->NL/loopNum, score_ave/loopNum);
	}

	free_L_Bip2(hybrid_result);
	free_Bip2(seti1);
	free_Bip2(seti2);
	free(score);

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
