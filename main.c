//#define NDEBUG  //for assert
#include <stdio.h>
//#include <assert.h>
#include <time.h>
//#include <stdlib.h>
#include "inc/linefile/i3linefile.h"
#include "inc/compact/bip3i.h"
#include "inc/linefile/iilinefile.h"
//#include "inc/linefile/iidlinefile.h"
#include "inc/compact/bip2.h"
//#include "inc/compact/iidnet.h"
//#include "inc/utility/error.h"
#include "inc/utility/random.h"
//#include "inc/utility/hashtable.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	char *netfilename;
	if (argc == 1) {
		netfilename = "data/movielen/movielens.txt";
	}
	if (argc == 2) {
		netfilename = argv[1];
	}

	//printf("%ld\n", t);
	unsigned long init[4]={t, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);

	struct i3LineFile *file = create_i3LineFile(netfilename);
	printf("%ld\n", file->linesNum);
	//print_i3LineFile(file, "Results/xx");
	struct Bip3i *bipi1 = create_Bip3i(file, 1);
	printf("%ld\n", bipi1->edgesNum);

	struct Bip3i *bipi2 = create_Bip3i(file, 0);
	printf("%ld\n", bipi2->edgesNum);

	//int i;
	//for (i=0; i<bipi1->count[1]; ++i) {
	//	printf("1 %d\n", bipi1->id[1][i]);
	//}

	struct iiLineFile *filex = create_iiLineFile(netfilename);
	struct Bip2 *b1 = create_Bip2(filex, 1);
	struct Bip2 *b2 = create_Bip2(filex, 0);
	verifyBip2(b1, b2);

	verifyBip3i(bipi1, bipi2);


	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
