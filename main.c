//#define NDEBUG  //for assert
#include <stdio.h>
#include <time.h>
#include "inc/linefile/i5linefile.h"
#include "inc/linefile/iilinefile.h"
#include "inc/linefile/iidlinefile.h"
#include "inc/compact/bip2.h"
#include "inc/compact/iidnet.h"
#include "inc/utility/error.h"
#include "inc/utility/hashtable.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	char *netfilename, *simfilename;
	if (argc == 1) {
		netfilename = "data/movielen/movielens.txt";
	}
	if (argc == 2) {
		netfilename = argv[1];
		simfilename = NULL;
	}
	if (argc == 3) {
		netfilename = argv[1];
		simfilename = argv[2];
	}

	//unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
	//init_by_array_MersenneTwister(init, length);

	//divide file into two part: train and test.
	//train will contain every user and every item.
	//but test maybe not.
	struct iiLineFile *net_file = create_iiLineFile(netfilename);
	struct Bip2 *seti1 = create_Bip2(net_file, 1);
	struct Bip2 *seti2 = create_Bip2(net_file, 0);
	struct iiLineFile *n2file = divide_Bip2(seti1, seti2, 0.1);
	free_iiLineFile(net_file);
	free_Bip2(seti1);
	free_Bip2(seti2);

	struct Bip2 *traini1= create_Bip2(n2file + 1, 1);
	struct Bip2 *traini2 = create_Bip2(n2file + 1, 0);
	//similarity_Bip2(traini1, traini2, "Results/netflix_similarity");
	struct Bip2 *testi1 = create_Bip2(n2file, 1);
	struct Bip2 *testi2 = create_Bip2(n2file, 0);
	free_2_iiLineFile(n2file);

	//the similarity is get from traini1
	//struct iidLineFile *similarity = create_iidLineFile("data/netflix_similarity");
	struct iidLineFile *similarity = create_iidLineFile(simfilename);
	struct iidNet *trainSim = create_iidNet(similarity);
	free_iidLineFile(similarity);

	//recommendation
	recovery_probs_Bip2(traini1, traini2, testi1, testi2, trainSim);
	//recovery_heats_Bip2(trainset1, trainset2, testseti1, testseti2);
	//recovery_grank_Bip2(trainset1, trainset2, testseti1, testseti2);

	free_iidNet(trainSim);
	free_Bip2(traini1);
	free_Bip2(traini2);
	free_Bip2(testi1);
	free_Bip2(testi2);

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
