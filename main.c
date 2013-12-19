/**
 * ./run data/movielens/movielens_2c 1 0.2
 * ./run data/netflix/netflix_2c 1 0.2 
 *
 */
//#define NDEBUG  //for assert
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "inc/linefile/i3linefile.h"
#include "inc/linefile/iidlinefile.h"
#include "inc/compact/bip3i.h"
#include "inc/compact/iidnet.h"
#include "inc/utility/error.h"
#include "inc/utility/random.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	char *netfilename;
	double lambda; 
	int loopNum;
	if (argc == 1) {
		netfilename = "data/movielens/movielens_2c";
		loopNum = 1;
		lambda = 0.2;
	}
	if (argc == 4) {
		netfilename = argv[1];
		char *pEnd;
		loopNum = strtol(argv[2], &pEnd, 10);
		lambda = strtod(argv[3], &pEnd);
	}

	//printf("%ld\n", t);
	unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);

	//divide file into two part: train and test.
	//train will contain every user and every item.
	//but test maybe not.
	struct i3LineFile *net_file = create_i3LineFile(netfilename);
	struct Bip3i *seti1 = create_Bip3i(net_file, 1);
	struct Bip3i *seti2 = create_Bip3i(net_file, 0);
	int i;

	struct L_Bip3i *hybrid_result = create_L_Bip3i();

	for (i=0; i<loopNum; ++i) {
		struct i3LineFile *n2file = divide_Bip3i(seti1, seti2, 0.1);

		struct Bip3i *traini1= create_Bip3i(n2file + 1, 1);
		struct Bip3i *traini2 = create_Bip3i(n2file + 1, 0);
		struct Bip3i *testi1 = create_Bip3i(n2file, 1);
		struct Bip3i *testi2 = create_Bip3i(n2file, 0);
		free_2_i3LineFile(n2file);

		//the similarity is get from traini1
		struct iidLineFile *similarity = similarity_realtime_Bip3i(traini1, traini2);
		struct iidNet *trainSim = create_iidNet(similarity);
		free_iidLineFile(similarity);

		//recommendation
		struct L_Bip3i *r4 = hybrid_Bip3i(traini1, traini2, testi1, testi2, trainSim, lambda);

		hybrid_result->R +=  r4->R;
		hybrid_result->PL += r4->PL;
		hybrid_result->HL += r4->HL;
		hybrid_result->IL += r4->IL;
		hybrid_result->NL += r4->NL;

		free_iidNet(trainSim);
		free_Bip3i(traini1);
		free_Bip3i(traini2);
		free_Bip3i(testi1);
		free_Bip3i(testi2);
		free_L_Bip3i(r4);
	}
	
	printf("average:\n");
	printf("hybrid\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, hybrid_result->R/loopNum, hybrid_result->PL/loopNum, hybrid_result->IL/loopNum, hybrid_result->HL/loopNum, hybrid_result->NL/loopNum);

	free_i3LineFile(net_file);
	free_Bip3i(seti1);
	free_Bip3i(seti2);
	free_L_Bip3i(hybrid_result);

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
