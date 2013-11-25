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
	char *netfilename, *scorefilename;
	if (argc == 1) {
		netfilename = "data/movielen/movielen2";
		scorefilename = "data/movielen/movielens.txt";
	}
	else if (argc == 3) {
		netfilename = argv[1];
		scorefilename = argv[2];
	}
	else {
		printf("wrong argc\n");
		return 0;
	}

	//printf("%ld\n", t);
	unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);

	struct i3LineFile *scorefile = create_i3LineFile(scorefilename);
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
	//FILE *fp = fopen("Results/score", "w");
	//int ii;
	//for (ii=0; ii<scorei2->maxId + 1; ++ii) {
	//	fprintf(fp, "%d, %f\n", ii, score[ii]);
	//}
	//fclose(fp);
	//return 0;
	free_Bip3i(scorei2);

	//divide file into two part: train and test.
	//train will contain every user and every item.
	//but test maybe not.
	struct iiLineFile *net_file = create_iiLineFile(netfilename);
	struct Bip2 *seti1 = create_Bip2(net_file, 1);
	struct Bip2 *seti2 = create_Bip2(net_file, 0);

	struct L_Bip2 *hybrid_result = create_L_Bip2(); 
	struct L_Bip2 *score_hybrid_result = create_L_Bip2(); 

	int loopNum = 100;
	int k;
	double lambda;
	for (k=0; k<101; ++k) {
		lambda = k*0.01;
		clean_L_Bip2(hybrid_result);
		clean_L_Bip2(score_hybrid_result);
		double score_ave = 0;
		double score_ave_2 = 0;
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

			//recommendation
			//struct L_Bip2 *r1 = probs_Bip2(traini1, traini2, testi1, testi2, trainSim);
			//struct L_Bip2 *r2 = HNBI_Bip2(traini1, traini2, testi1, testi2, trainSim, -0.8);
			//struct L_Bip2 *r3 = RENBI_Bip2(traini1, traini2, testi1, testi2, trainSim, -0.75);
			//heats_Bip2(traini1, traini2, testi1, testi2, trainSim);
			struct L_Bip2 *r4 = hybrid_Bip2(traini1, traini2, testi1, testi2, trainSim, lambda);
			struct L_Bip2 *r5 = score_hybrid_Bip2(traini1, traini2, testi1, testi2, trainSim, lambda, score, lambda);

			//probs_result->R += r1->R;
			//probs_result->PL += r1->PL;
			//probs_result->HL += r1->HL;
			//probs_result->IL += r1->IL;
			//probs_result->NL += r1->NL;
			//HNBI_result->R +=  r2->R;
			//HNBI_result->PL += r2->PL;
			//HNBI_result->HL += r2->HL;
			//HNBI_result->IL += r2->IL;
			//HNBI_result->NL += r2->NL;
			//RENBI_result->R +=  r3->R;
			//RENBI_result->PL += r3->PL;
			//RENBI_result->HL += r3->HL;
			//RENBI_result->IL += r3->IL;
			//RENBI_result->NL += r3->NL;
			hybrid_result->R +=  r4->R;
			hybrid_result->PL += r4->PL;
			hybrid_result->HL += r4->HL;
			hybrid_result->IL += r4->IL;
			hybrid_result->NL += r4->NL;
			score_hybrid_result->R +=  r5->R;
			score_hybrid_result->PL += r5->PL;
			score_hybrid_result->HL += r5->HL;
			score_hybrid_result->IL += r5->IL;
			score_hybrid_result->NL += r5->NL;
			int L = r4->L;
			int *topL = r4->topL;
			//int i1;
			//for (i1 = 0; i1<traini1->maxId + 1; ++i1) { //each user
			//	if (traini1->count[i1] > 0 && testi1->count[i1] > 0) {
			//		int i2;
			//		for (i2 = 0; i2 < traini1->count[i1]; ++i2) {
			//			int id = list[i1*LNum + i2];
			//			score_ave += score[id];
			//		}
			//	}
			//}
			long len = L*(traini1->maxId + 1);
			for (j=0; j<L*(traini1->maxId + 1); ++j) {
				score_ave += score[topL[j]]/len;
				score_ave_2 += score[r5->topL[j]]/len;
			}
			free_iidNet(trainSim);
			free_Bip2(traini1);
			free_Bip2(traini2);
			free_Bip2(testi1);
			free_Bip2(testi2);
			//free(r1); free(r2); free(r3); 
			free_L_Bip2(r4);
			free_L_Bip2(r5);
		}
		printf("      hybrid\tlambda: %f, loopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f, Score: %f\n", lambda, loopNum, hybrid_result->R/loopNum, hybrid_result->PL/loopNum, hybrid_result->IL/loopNum, hybrid_result->HL/loopNum, hybrid_result->NL/loopNum, score_ave/loopNum);
		printf("score_hybrid\tlambda: %f, loopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f, Score: %f\n", lambda, loopNum, score_hybrid_result->R/loopNum, score_hybrid_result->PL/loopNum, score_hybrid_result->IL/loopNum, score_hybrid_result->HL/loopNum, score_hybrid_result->NL/loopNum, score_ave_2/loopNum);
	}
	free_L_Bip2(hybrid_result);
	free_L_Bip2(score_hybrid_result);
	free(score);
	
	//printf("probs\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", probs_result->R/loopNum, probs_result->PL/loopNum, probs_result->IL/loopNum, probs_result->HL/loopNum, probs_result->NL/loopNum);
	//printf("HNBI\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", HNBI_result->R/loopNum, HNBI_result->PL/loopNum, HNBI_result->IL/loopNum, HNBI_result->HL/loopNum, HNBI_result->NL/loopNum);
	//printf("RENBI\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", RENBI_result->R/loopNum, RENBI_result->PL/loopNum, RENBI_result->IL/loopNum, RENBI_result->HL/loopNum, RENBI_result->NL/loopNum);

	free_iiLineFile(net_file);
	free_Bip2(seti1);
	free_Bip2(seti2);
	//free(probs_result);
	//free(HNBI_result);
	//free(RENBI_result);

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
