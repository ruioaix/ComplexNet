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


	struct iiLineFile *testfile = create_iiLineFile("data/movielen/movielen0.1");
	struct iiLineFile *trainfile = create_iiLineFile("data/movielen/movielen0.9");

	struct Bip2 *testset = create_Bip2(testfile, 0);
	struct Bip2 *trainset = create_Bip2(trainfile, 0);

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
