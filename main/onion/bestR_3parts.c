/**
 * onion model knn implementation
 *
 * 1, read a biparte net, and divide it into two parts: trainset,testset.
 * 2, for each user in testset, first I use the first two step of mass diffusion, then 
 *    I will use the user-similarity to decide how many similar users I will use on the third step.
 *    actually, for one user, I have to do many calculations.
 *    for example, if one user has 100 similar users, I will need to calculate 100 times.
 *    	the first time: I use the top 1 user to calculate.
 *    	the second time: I use the top 2 users to calculate.
 *    	the third time: I use the top 3 users to calculate.
 *    	...
 *    	the 100 time: I use all 100 users.
 *    and this is only one user.
 *    the result is that: one user to one simliar users' number.
 * 3, use the new data to diffusion, get rankscore and all other things. and use mass diffusion too, get compared.
 *
 */

//#define NDEBUG  //for assert
#include <stdio.h>

#include <assert.h>
#include <time.h>
//#include <math.h>
#include <stdlib.h>
//#include <string.h>
#include "iilinefile.h"
#include "iidlinefile.h"
#include "bip.h"
#include "error.h"
#include "mt_random.h"

void set_RandomSeed(void) {
	time_t t=time(NULL);
	unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);
}

void print_time(void) {
	time_t t=time(NULL); 
	printf("%s\n", ctime(&t)); 
	fflush(stdout);
}

void create_2dataset(struct Bipii *neti1, struct Bipii *neti2, double rate, struct Bipii **A1, struct Bipii **A2, struct Bipii **B1, struct Bipii **B2) {
	struct iiLineFile *first, *second;
	divide_Bipii(neti1, neti2, rate, &first, &second);
	//A is big train set. B is big test set.
	*A1 = create_Bipii(second, 1);
	*A2 = create_Bipii(second, 0);
	*B1 = create_Bipii(first, 1);
	*B2 = create_Bipii(first, 0);
	free_iiLineFile(first);
	free_iiLineFile(second);
}

void get_UserSimilarity(struct Bipii *traini1, struct Bipii *traini2, struct iidNet **userSim) {
	struct iidLineFile *userSimilarityfile = similarity_Bipii(traini1, traini2, 1);
	*userSim = create_iidNet(userSimilarityfile);
	free_iidLineFile(userSimilarityfile);
}

void get_ItemSimilarity(struct Bipii *traini1, struct Bipii *traini2, struct iidNet **itemSim) {
	struct iidLineFile *itemSimilarityfile = similarity_Bipii(traini1, traini2, 0);
	*itemSim = create_iidNet(itemSimilarityfile);
	free_iidLineFile(itemSimilarityfile);
}

void test_ArgcArgv(int argc, char **argv, char **netfilename) {
	if (argc == 1) {
		*netfilename = "data/delicious/delicious_2c_sub1000";
		//*netfilename = "data/movielens/movielens_2c";
		//*colK = 0.2;	
	}
	else if (argc == 2) {
		*netfilename = argv[1];
		//char *p;
		//*colK = strtod(argv[2], &p);
	}
	else {
		isError("wrong argc, argv.\n");
	}
}

void print_bestR(int *bestR, int length, char *filename) {
	FILE *fp = fopen(filename, "w");
	fileError(fp, filename);
	int i;
	for (i=0; i<length; ++i) {
		fprintf(fp, "%d\t%d\n", i, bestR[i]);
	}
	fclose(fp);
}

int main(int argc, char **argv)
{
	print_time();
	set_RandomSeed();

	char *netfilename;
	test_ArgcArgv(argc, argv, &netfilename); 

	struct iiLineFile *netfile = create_iiLineFile(netfilename);
	struct Bipii *neti1 = create_Bipii(netfile, 1);
	struct Bipii *neti2 = create_Bipii(netfile, 0);
	free_iiLineFile(netfile);

	struct Bipii *A1, *A2, *B1, *B2;
	create_2dataset(neti1, neti2, 0.1, &A1, &A2, &B1, &B2);
	free_Bipii(neti1);
	free_Bipii(neti2);

	struct iidNet *AuserSim, *AitemSim=NULL;
	get_UserSimilarity(A1, A2, &AuserSim);
	get_ItemSimilarity(A1, A2, &AitemSim);
	sort_desc_iidNet(AuserSim);

	//C is train set in A, D is test set in A.
	struct Bipii *C1, *C2, *D1, *D2;
	create_2dataset(A1, A2, 0.13, &C1, &C2, &D1, &D2);
	struct iidNet *CuserSim, *CitemSim=NULL;
	get_UserSimilarity(C1, C2, &CuserSim);
	get_ItemSimilarity(C1, C2, &CitemSim);
	sort_desc_iidNet(CuserSim);

	//int *ADbestR = mass_GetBestR_Bipii(A1, A2, D1, D2, AuserSim);
	//print_bestR(ADbestR, A1->maxId + 1, "ADbestR");
	int *CDbestR = mass_GetBestR_Bipii(C1, C2, D1, D2, CuserSim);
	print_bestR(CDbestR, C1->maxId + 1, "CDbestR");
	int *CBbestR = mass_GetBestR_Bipii(C1, C2, B1, B2, CuserSim);
	print_bestR(CBbestR, C1->maxId + 1, "CBbestR");
	//int *ABbestR = mass_GetBestR_Bipii(A1, A2, B1, B2, AuserSim);
	//print_bestR(ABbestR, A1->maxId + 1, "ABbestR");


	struct Metrics_Bipii *bestR_result;
	bestR_result = mass_bestR_Bipii(C1, C2, B1, B2, CitemSim, CuserSim, CBbestR);
	printf("bestR-cb,best\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", bestR_result->R, bestR_result->PL, bestR_result->IL, bestR_result->HL, bestR_result->NL);
    bestR_result = mass_bestR_Bipii(C1, C2, B1, B2, CitemSim, CuserSim, CDbestR);
	printf("bestR-cb,cdbest\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", bestR_result->R, bestR_result->PL, bestR_result->IL, bestR_result->HL, bestR_result->NL);
	bestR_result = mass_bestR_Bipii(A1, A2, B1, B2, AitemSim, AuserSim, CDbestR);
	printf("bestR-ab,cdbest\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", bestR_result->R, bestR_result->PL, bestR_result->IL, bestR_result->HL, bestR_result->NL);
	free_MetricsBipii(bestR_result);

	free_Bipii(A1);
	free_Bipii(A2);
	free_Bipii(B1);
	free_Bipii(B2);
	free_Bipii(C1);
	free_Bipii(C2);
	free_Bipii(D1);
	free_Bipii(D2);

	free_iidNet(AuserSim);
	free_iidNet(AitemSim);
	free_iidNet(CuserSim);
	free_iidNet(CitemSim);
	//free_L_Bip(mass_result);
	//free_L_Bip(simcut_result);
	
	print_time();
	return 0;
}
