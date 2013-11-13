#define NDEBUG  //for assert
#include "inc/linefile/ilinefile.h" //for readFileLBL;
#include "inc/linefile/iilinefile.h" //for readFileLBL;
#include "inc/linefile/i3linefile.h" //for readFileLBL;
#include "inc/linefile/iid3linefile.h" //for readFileLBL;
#include "inc/compact/net.h" //for buildDNet;
#include "inc/matrix/pspipr.h" //for buildDNet;
#include "inc/matrix/snapshot.h" //for buildDNet;
#include "inc/utility/error.h"
#include "inc/utility/sort.h"
#include "inc/hashtable/dmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
