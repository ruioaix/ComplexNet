//#define NDEBUG  //for assert
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "inc/linefile/iilinefile.h"
#include "inc/linefile/iidlinefile.h"
#include "inc/compact/bip2.h"
#include "inc/utility/error.h"
#include "inc/utility/random.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	char *netfilename;
	int loopNum, stepNum, topstepLen;
	double stepbegin, stepLen;
	if (argc == 1) {
		netfilename = "data/movielen/movielen2";
		loopNum = 2;
		stepbegin = 0;
		stepLen = 0.01;
		stepNum = 100;
		//topstepLen = 10;
	}
	else if (argc == 7) {
		netfilename = argv[1];
		char *pEnd;
		loopNum = strtol(argv[2], &pEnd, 10);
		stepbegin = strtod(argv[3], &pEnd);
		stepLen = strtod(argv[4], &pEnd);
		stepNum = strtol(argv[5], &pEnd, 10);
		//topstepLen = strtol(argv[6], &pEnd, 10);
	}
	else {
		printf("wrong argc\n");
		return 0;
	}

	//random number init, make sure that random number generated differently every time program run.
	unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);

	//get netfile, build net.
	struct iiLineFile *netfile = create_iiLineFile(netfilename);
	struct Bip2 *bipi1 = create_Bip2(netfile, 1);
	struct Bip2 *bipi2 = create_Bip2(netfile, 0);
	free_iiLineFile(netfile);

	topstepLen = (bipi1->maxId + 1)/stepNum;

	struct L_Bip2 *omass_result = create_L_Bip2();
	struct L_Bip2 *tmass_result = create_L_Bip2();

	int i, j;
	for (i=0; i<stepNum; ++i) {
		double orate = i*stepLen + stepbegin;
		int topR = i*topstepLen;

		clean_L_Bip2(omass_result);
		clean_L_Bip2(tmass_result);

		for (j=0; j<loopNum; ++j) {

			struct iiLineFile *twofile = divide_Bip2(bipi1, bipi2, 0.1);
			struct Bip2 *traini1 = create_Bip2(twofile + 1, 1);
			struct Bip2 *traini2 = create_Bip2(twofile + 1, 0);
			struct Bip2 *testi1 = create_Bip2(twofile, 1);
			struct Bip2 *testi2 = create_Bip2(twofile, 0);
			free_2_iiLineFile(twofile);

			struct iidLineFile *user_similarity_file = similarity_realtime_Bip2(traini1, traini2, 1);
			struct iidNet *userSim= create_iidNet(user_similarity_file);
			sort_desc_iidNet(userSim);
			free_iidLineFile(user_similarity_file);

			struct iidLineFile *item_similarity_file = similarity_realtime_Bip2(traini1, traini2, 0);
			struct iidNet *itemSim= create_iidNet(item_similarity_file);
			free_iidLineFile(item_similarity_file);

			struct L_Bip2 *r1 = onion_mass_Bip2(traini1, traini2, testi1, testi2, itemSim, userSim, orate);
			struct L_Bip2 *r2 = topR_probs_Bip2(traini1, traini2, testi1, testi2, itemSim, userSim, topR);

			omass_result->R +=  r1->R;
			omass_result->PL += r1->PL;
			omass_result->HL += r1->HL;
			omass_result->IL += r1->IL;
			omass_result->NL += r1->NL;
			tmass_result->R +=  r2->R;
			tmass_result->PL += r2->PL;
			tmass_result->HL += r2->HL;
			tmass_result->IL += r2->IL;
			tmass_result->NL += r2->NL;

			free_Bip2(traini1);
			free_Bip2(traini2);
			free_Bip2(testi1);
			free_Bip2(testi2);
			free_iidNet(userSim);
			free_iidNet(itemSim);
			free_L_Bip2(r1);
			free_L_Bip2(r2);
		}

		printf("onion_mass\torate: %f, loopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", orate, loopNum, omass_result->R/loopNum, omass_result->PL/loopNum, omass_result->IL/loopNum, omass_result->HL/loopNum, omass_result->NL/loopNum);
		printf("topR_mass\ttopR: %d, loopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", topR, loopNum, tmass_result->R/loopNum, tmass_result->PL/loopNum, tmass_result->IL/loopNum, tmass_result->HL/loopNum, tmass_result->NL/loopNum);


	}


	free_L_Bip2(omass_result);
	free_L_Bip2(tmass_result);
	free_Bip2(bipi1);
	free_Bip2(bipi2);


	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
