/**
 * ./run data/movielens/movielens_2c 1 -0.8 -0.75 0.2
 * ./run data/netflix/netflix_2c 1 -0.7 -0.75 0.2 
 *
 */

//#define NDEBUG  //for assert
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "iilinefile.h"
#include "iidlinefile.h"
#include "bip.h"
#include "iidnet.h"
#include "error.h"
#include "mt_random.h"

void print_time(void) {
	time_t t=time(NULL); 
	printf("%s\n", ctime(&t)); 
	fflush(stdout);
}

void set_RandomSeed(void) {
	time_t t=time(NULL);
	unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);
}


int main(int argc, char **argv)
{
	print_time();
	set_RandomSeed();
	
	char *netfilename;
	double HNBI_param, RENBI_param, hybrid_param; 
	int loopNum;
	if (argc == 1) {
		netfilename = "data/movielens/movielens_2c";
		loopNum = 1;
		HNBI_param = -0.8;
		RENBI_param= -0.75;
		hybrid_param = 0.2;
	}
	if (argc == 6) {
		netfilename = argv[1];
		char *pEnd;
		loopNum = strtol(argv[2], &pEnd, 10);
		HNBI_param = strtod(argv[3], &pEnd);
		RENBI_param= strtod(argv[4], &pEnd);
		hybrid_param = strtod(argv[5], &pEnd);
	}

	//divide file into two part: train and test.
	//train will contain every user and every item.
	//but test maybe not.
	struct iiLineFile *net_file = create_iiLineFile(netfilename);
	struct Bipii *seti1 = create_Bipii(net_file, 1);
	struct Bipii *seti2 = create_Bipii(net_file, 0);
	int i;

	struct Metrics_Bipii *mass_result = create_MetricsBipii();
	struct Metrics_Bipii *heats_result = create_MetricsBipii();
	struct Metrics_Bipii *HNBI_result = create_MetricsBipii();
	struct Metrics_Bipii *RENBI_result = create_MetricsBipii();
	struct Metrics_Bipii *hybrid_result = create_MetricsBipii();

	for (i=0; i<loopNum; ++i) {
		struct iiLineFile *testfile, *trainfile;
		divide_Bipii(seti1, seti2, 0.1, &testfile, &trainfile);

		struct Bipii *traini1= create_Bipii(trainfile, 1);
		struct Bipii *traini2 = create_Bipii(trainfile, 0);
		struct Bipii *testi1 = create_Bipii(testfile, 1);
		struct Bipii *testi2 = create_Bipii(testfile, 0);
		free_iiLineFile(trainfile);
		free_iiLineFile(testfile);

		//the similarity is get from traini1
		struct iidLineFile *similarity = similarity_Bipii(traini1, traini2, 0);
		struct iidNet *trainSim = create_iidNet(similarity);
		free_iidLineFile(similarity);

		//recommendation
		struct Metrics_Bipii *r1 = mass_Bipii(traini1, traini2, testi1, testi2, trainSim);
		struct Metrics_Bipii *r11= heats_Bipii(traini1, traini2, testi1, testi2, trainSim);
		struct Metrics_Bipii *r2 = HNBI_Bipii(traini1, traini2, testi1, testi2, trainSim, HNBI_param);
		struct Metrics_Bipii *r3 = RENBI_Bipii(traini1, traini2, testi1, testi2, trainSim, RENBI_param);
		struct Metrics_Bipii *r4 = hybrid_Bipii(traini1, traini2, testi1, testi2, trainSim, hybrid_param);

		mass_result->R += r1->R;
		mass_result->PL += r1->PL;
		mass_result->HL += r1->HL;
		mass_result->IL += r1->IL;
		mass_result->NL += r1->NL;
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
		free_Bipii(traini1);
		free_Bipii(traini2);
		free_Bipii(testi1);
		free_Bipii(testi2);
		free_MetricsBipii(r1);
		free_MetricsBipii(r11);
		free_MetricsBipii(r2);
		free_MetricsBipii(r3);
		free_MetricsBipii(r4);
	}
	
	printf("average:\n");
	printf("mass\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, mass_result->R/loopNum, mass_result->PL/loopNum, mass_result->IL/loopNum, mass_result->HL/loopNum, mass_result->NL/loopNum);
	printf("HNBI\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, HNBI_result->R/loopNum, HNBI_result->PL/loopNum, HNBI_result->IL/loopNum, HNBI_result->HL/loopNum, HNBI_result->NL/loopNum);
	printf("RENBI\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, RENBI_result->R/loopNum, RENBI_result->PL/loopNum, RENBI_result->IL/loopNum, RENBI_result->HL/loopNum, RENBI_result->NL/loopNum);
	printf("hybrid\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, hybrid_result->R/loopNum, hybrid_result->PL/loopNum, hybrid_result->IL/loopNum, hybrid_result->HL/loopNum, hybrid_result->NL/loopNum);
	printf("heats\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, heats_result->R/loopNum, heats_result->PL/loopNum, heats_result->IL/loopNum, heats_result->HL/loopNum, heats_result->NL/loopNum);

	free_iiLineFile(net_file);
	free_Bipii(seti1);
	free_Bipii(seti2);
	free_MetricsBipii(mass_result);
	free_MetricsBipii(heats_result);
	free_MetricsBipii(HNBI_result);
	free_MetricsBipii(RENBI_result);
	free_MetricsBipii(hybrid_result);

	print_time();
	return 0;
}
