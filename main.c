//#define NDEBUG  //for assert
#include "inc/complexnet_file.h" //for readFileLBL;
#include "inc/complexnet_dnet.h" //for buildDNet;
#include "inc/complexnet_dtnet.h" //for buildDNet;
#include "inc/complexnet_random.h"
#include "inc/complexnet_hashtable.h" //for buildDNet;
#include "inc/complexnet_threadpool.h"
#include "inc/complexnet_error.h"
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);

	//create thread pool.
	int threadMax = 10;
	createThreadPool(threadMax);
	
	//destroy thread pool.
	destroyThreadPool();

	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
