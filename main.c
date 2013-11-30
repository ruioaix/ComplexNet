//#define NDEBUG  //for assert
#include <stdio.h>
//#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "inc/linefile/i3linefile.h"
#include "inc/compact/bip3i.h"
//#include "inc/utility/error.h"
#include "inc/utility/random.h"
//#include "inc/utility/hashtable.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	char *netfilename;
	int loopNum;
	if (argc == 1) {
		netfilename = "data/movielen/movielens.txt";
		loopNum = 10;
	}
	else if (argc == 3) {
		netfilename = argv[1];
		char *pEnd;
		loopNum = strtol(argv[2], &pEnd, 10);
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
	struct i3LineFile *twofile = divide_Bip3i(neti1, neti2, 0.1);
	free_i3LineFile(netfile);
	free_Bip3i(neti1);
	free_Bip3i(neti2);

	struct Bip3i *traini1 = create_Bip3i(twofile + 1, 1);
	struct Bip3i *traini2 = create_Bip3i(twofile + 1, 0);
	struct Bip3i *testi1 = create_Bip3i(twofile, 1);
	struct Bip3i *testi2 = create_Bip3i(twofile, 0);
	free_2_i3LineFile(twofile);

	struct iidLineFile *simfile = similarity_realtime_Bip3i(traini1, traini2);
	struct iidNet *trainSim = create_iidNet(simfile);
	free_iidLineFile(simfile);

	double theta = 0;
	struct L_Bip3i *smass_result = s_mass_Bip3i(traini1, traini2, testi1, testi2, trainSim, theta);
	
	free_Bip3i(traini1);
	free_Bip3i(traini2);
	free_Bip3i(testi1);
	free_Bip3i(testi2);
	free_iidNet(trainSim);

	free_L_Bip3i(smass_result);

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
