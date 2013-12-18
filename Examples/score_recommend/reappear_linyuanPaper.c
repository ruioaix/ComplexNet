//#define NDEBUG  //for assert
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "inc/linefile/i5linefile.h"
#include "inc/linefile/iilinefile.h"
#include "inc/linefile/iidlinefile.h"
#include "inc/compact/bip2.h"
#include "inc/compact/iidnet.h"
#include "inc/utility/error.h"
#include "inc/utility/random.h"
#include "inc/utility/hashtable.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	char *netfilename;
	if (argc == 1) {
		netfilename = "data/movielen/movielen2";
	}
	if (argc == 2) {
		netfilename = argv[1];
	}

	//printf("%ld\n", t);
	unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);

	//divide file into two part: train and test.
	//train will contain every user and every item.
	//but test maybe not.
	struct iiLineFile *net_file = create_iiLineFile(netfilename);
	struct Bip2 *seti1 = create_Bip2(net_file, 1);
	struct Bip2 *seti2 = create_Bip2(net_file, 0);
	int i;

	struct L_Bip2 *probs_result = create_L_Bip2();
	struct L_Bip2 *heats_result = create_L_Bip2();
	struct L_Bip2 *HNBI_result = create_L_Bip2();
	struct L_Bip2 *RENBI_result = create_L_Bip2();
	struct L_Bip2 *hybrid_result = create_L_Bip2();

	int loopNum = 1;
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
		struct L_Bip2 *r1 = probs_Bip2(traini1, traini2, testi1, testi2, trainSim);
		struct L_Bip2 *r11= heats_Bip2(traini1, traini2, testi1, testi2, trainSim);
		struct L_Bip2 *r2 = HNBI_Bip2(traini1, traini2, testi1, testi2, trainSim, -0.8);
		struct L_Bip2 *r3 = RENBI_Bip2(traini1, traini2, testi1, testi2, trainSim, -0.75);
		struct L_Bip2 *r4 = hybrid_Bip2(traini1, traini2, testi1, testi2, trainSim, 0.2);

		probs_result->R += r1->R;
		probs_result->PL += r1->PL;
		probs_result->HL += r1->HL;
		probs_result->IL += r1->IL;
		probs_result->NL += r1->NL;
		heats_result->R += r11->R;
		heats_result->PL += r11->PL;
		heats_result->HL += r11->HL;
		heats_result->IL += r11->IL;
		heats_result->NL += r11->NL;
		HNBI_result->R +=  r2->R;
		HNBI_result->PL += r2->PL;
		HNBI_result->HL += r2->HL;
		HNBI_result->IL += r2->IL;
		HNBI_result->NL += r2->NL;
		RENBI_result->R +=  r3->R;
		RENBI_result->PL += r3->PL;
		RENBI_result->HL += r3->HL;
		RENBI_result->IL += r3->IL;
		RENBI_result->NL += r3->NL;
		hybrid_result->R +=  r4->R;
		hybrid_result->PL += r4->PL;
		hybrid_result->HL += r4->HL;
		hybrid_result->IL += r4->IL;
		hybrid_result->NL += r4->NL;

		free_iidNet(trainSim);
		free_Bip2(traini1);
		free_Bip2(traini2);
		free_Bip2(testi1);
		free_Bip2(testi2);
		free_L_Bip2(r1);
		free_L_Bip2(r11);
		free_L_Bip2(r2);
		free_L_Bip2(r3);
		free_L_Bip2(r4);
	}
	
	printf("average:\n");
	printf("probs\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", probs_result->R/loopNum, probs_result->PL/loopNum, probs_result->IL/loopNum, probs_result->HL/loopNum, probs_result->NL/loopNum);
	printf("heats\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", heats_result->R/loopNum, heats_result->PL/loopNum, heats_result->IL/loopNum, heats_result->HL/loopNum, heats_result->NL/loopNum);
	printf("HNBI\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", HNBI_result->R/loopNum, HNBI_result->PL/loopNum, HNBI_result->IL/loopNum, HNBI_result->HL/loopNum, HNBI_result->NL/loopNum);
	printf("RENBI\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", RENBI_result->R/loopNum, RENBI_result->PL/loopNum, RENBI_result->IL/loopNum, RENBI_result->HL/loopNum, RENBI_result->NL/loopNum);
	printf("hybrid\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", hybrid_result->R/loopNum, hybrid_result->PL/loopNum, hybrid_result->IL/loopNum, hybrid_result->HL/loopNum, hybrid_result->NL/loopNum);

	free_iiLineFile(net_file);
	free_Bip2(seti1);
	free_Bip2(seti2);
	free_L_Bip2(probs_result);
	free_L_Bip2(heats_result);
	free_L_Bip2(HNBI_result);
	free_L_Bip2(RENBI_result);
	free_L_Bip2(hybrid_result);

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
