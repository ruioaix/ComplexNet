#define NDEBUG  //for assert
#include <stdio.h>
#include <time.h>
#include "inc/linefile/i5linefile.h"
#include "inc/linefile/iilinefile.h"
#include "inc/compact/bip2.h"
#include "inc/utility/error.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);

	//struct iiLineFile *netflix = create_iiLineFile("data/netflix/netflix_pnas.txt");
	//struct iiLineFile *n2file = divide_iiLineFile(netflix, 0.1);
	//print_iiLineFile(n2file, "Results/netflix0.1");
	//print_iiLineFile(n2file+1, "Results/netflix0.9");
	//exit(0);


	//struct iiLineFile *test = create_iiLineFile("data/netflix/netflix0.1");
	//struct iiLineFile *train = create_iiLineFile("data/netflix/netflix0.1");
	//struct Bip2 *testseti1 = create_Bip2(test, 1);
	//struct Bip2 *testseti2 = create_Bip2(test, 0);
	//struct Bip2 *trainset1 = create_Bip2(train, 1);
	//struct Bip2 *trainset2 = create_Bip2(train, 0);

	//struct iiLineFile *testfile = create_iiLineFile("data/movielen/movielen0.1");
	//struct iiLineFile *trainfile = create_iiLineFile("data/movielen/movielen0.9");
	//struct Bip2 *testseti1 = create_Bip2(testfile, 1);
	//struct Bip2 *testseti2 = create_Bip2(testfile, 0);
	//struct Bip2 *trainset1 = create_Bip2(trainfile, 1);
	//struct Bip2 *trainset2 = create_Bip2(trainfile, 0);
	
	struct iiLineFile *movielens = create_iiLineFile("data/movielen/movielens.txt");
	struct iiLineFile *m2file = divide_iiLineFile(movielens, 0.1);
	m2file = divide_iiLineFile(movielens, 0.1);
	struct Bip2 *trainset1 = create_Bip2(m2file + 1, 1);
	struct Bip2 *trainset2 = create_Bip2(m2file + 1, 0);
	struct Bip2 *testseti1 = create_Bip2(m2file, 1);
	

	//double rank2 = recovery_Bip2_2(trainset1, trainset2, testseti1);
	recovery_Bip2(trainset1, trainset2, testseti1);

	/*
	long i;
	FILE *fp = fopen("Results/movielen2", "w");
	fileError(fp, "main");
	for (i=0; i<file->linesNum; ++i) {
		if (file->lines[i].i3 > 2) {
			fprintf(fp, "%d %d\n", file->lines[i].i1, file->lines[i].i2);
		}
	}
	fclose(fp);
	*/


	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
