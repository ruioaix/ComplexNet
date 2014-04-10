/**
*/

//#define NDEBUG  //for assert
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include "recommend.h"

void print_time(void) {
	time_t t=time(NULL); 
	printf("%s", ctime(&t)); 
	fflush(stdout);
}

int hash (char *method) {
	int i;
	int result=0;
	for (i=0; method[i] != '\0'; ++i) {
		result += method[i];
	}
	return result;
}

int main(int argc, char **argv)
{
	print_time();

	opterr = 0;

	int c;
	char *method=NULL;
	char *method_arg_1=NULL;
	char *method_arg_2=NULL;
	char *method_arg_3=NULL;
	while ((c=getopt(argc, argv, "x:y:z:m:"))!=-1) {
		switch (c) {
			case 'm':
				method = optarg;
				printf ("method is %s.\n", method);
				break;
			case 'x':
				method_arg_1 = optarg;
				break;
			case 'y':
				method_arg_2 = optarg;
				break;
			case 'z':
				method_arg_3 = optarg;
				break;
			case '?':
				if (optopt == 'm')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				return 1;
				break;
			default:
				abort();
		}
	}

	if (argc - optind != 1) {
		fprintf (stderr, "need one dataset file, no more, no less.\n");
		return 1;	
	}
	int methodInt;
	if (method) {
		methodInt = hash(method);
		printf("%s is %d\n", method, methodInt);
		switch (methodInt) {
			case 436: //mass
				mass(argv[optind]);
				break;
			case 533: //heats
				heats(argv[optind]);
				break;
			case 289: //HNBI
				HNBI(argv[optind], method_arg_1);
				break;
			case 368: //RENBI
				RENBI(argv[optind], method_arg_1);
				break;
			case 642: //hybrid
				hybrid(argv[optind], method_arg_1);
				break;
			default:
				printf("not a valid method.\n");
				return 1;
		}
	}

	return 0;
}
