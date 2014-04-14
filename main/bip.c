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
#include "base.h"
#include "linefile.h"
#include "bip.h"
#include "iidnet.h"
#include "mtprand.h"

int main(int argc, char **argv)
{
	print_time();
	set_timeseed_MTPR();
	
	char *netfilename;
	double HNBI_param, RENBI_param, hybrid_param; 
	int loopNum;
	if (argc == 1) {
		netfilename = "movielens_2c";
		loopNum = 2;
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
	struct LineFile *net_file = create_LineFile(netfilename, 1, 1, -1);
	struct iiBip *seti1 = create_iiBip(net_file, 1);
	struct iiBip *seti2 = create_iiBip(net_file, 2);
	int i;

	struct Metrics_iiBip *mass_result = create_MetricsiiBip();
	struct Metrics_iiBip *heats_result = create_MetricsiiBip();
	struct Metrics_iiBip *HNBI_result = create_MetricsiiBip();
	struct Metrics_iiBip *RENBI_result = create_MetricsiiBip();
	struct Metrics_iiBip *hybrid_result = create_MetricsiiBip();

	for (i=0; i<loopNum; ++i) {
		printf("\n");
		struct LineFile *testfile, *trainfile;
		divide_iiBip(seti1, seti2, 0.1, &testfile, &trainfile);

		struct iiBip *traini1= create_iiBip(trainfile, 1);
		struct iiBip *traini2 = create_iiBip(trainfile, 2);
		struct iiBip *testi1 = create_iiBip(testfile, 1);
		struct iiBip *testi2 = create_iiBip(testfile, 2);
		free_LineFile(trainfile);
		free_LineFile(testfile);

		//the similarity is get from traini1
		struct LineFile *similarity = similarity_iiBip(traini1, traini2, 2);
		struct iidNet *trainSim = create_iidNet(similarity);
		free_LineFile(similarity);

		//recommendation
		struct Metrics_iiBip *r1 = mass_iiBip(traini1, traini2, testi1, testi2, trainSim);
		struct Metrics_iiBip *r11= heats_iiBip(traini1, traini2, testi1, testi2, trainSim);
		struct Metrics_iiBip *r2 = HNBI_iiBip(traini1, traini2, testi1, testi2, trainSim, HNBI_param);
		struct Metrics_iiBip *r3 = RENBI_iiBip(traini1, traini2, testi1, testi2, trainSim, RENBI_param);
		struct Metrics_iiBip *r4 = hybrid_iiBip(traini1, traini2, testi1, testi2, trainSim, hybrid_param);

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
		free_iiBip(traini1);
		free_iiBip(traini2);
		free_iiBip(testi1);
		free_iiBip(testi2);
		free_MetricsiiBip(r1);
		free_MetricsiiBip(r11);
		free_MetricsiiBip(r2);
		free_MetricsiiBip(r3);
		free_MetricsiiBip(r4);
	}
	
	printf("RESULT:\n");
	printf("\tmass\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, mass_result->R/loopNum, mass_result->PL/loopNum, mass_result->IL/loopNum, mass_result->HL/loopNum, mass_result->NL/loopNum);
	printf("\tHNBI\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, HNBI_result->R/loopNum, HNBI_result->PL/loopNum, HNBI_result->IL/loopNum, HNBI_result->HL/loopNum, HNBI_result->NL/loopNum);
	printf("\tRENBI\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, RENBI_result->R/loopNum, RENBI_result->PL/loopNum, RENBI_result->IL/loopNum, RENBI_result->HL/loopNum, RENBI_result->NL/loopNum);
	printf("\thybrid\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, hybrid_result->R/loopNum, hybrid_result->PL/loopNum, hybrid_result->IL/loopNum, hybrid_result->HL/loopNum, hybrid_result->NL/loopNum);
	printf("\theats\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, heats_result->R/loopNum, heats_result->PL/loopNum, heats_result->IL/loopNum, heats_result->HL/loopNum, heats_result->NL/loopNum);

	free_LineFile(net_file);
	free_iiBip(seti1);
	free_iiBip(seti2);
	free_MetricsiiBip(mass_result);
	free_MetricsiiBip(heats_result);
	free_MetricsiiBip(HNBI_result);
	free_MetricsiiBip(RENBI_result);
	free_MetricsiiBip(hybrid_result);

	print_time();
	return 0;
}
