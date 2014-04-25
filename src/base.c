#include <stdio.h>

/********************************************************************************************************/
#include <time.h>
void print_time(void) {
	time_t t=time(NULL); 
	printf("%s", ctime(&t)); 
	(void)fflush(stdout);
}
void print_label(void) {
	printf("xxxx====xxxx\n");
	fflush(stdout);
}
/********************************************************************************************************/

/********************************************************************************************************/
#include <stddef.h> //for NULL
#include <stdlib.h> //for exit & EXIT_FAILURE
void fileError(FILE *fp, const char * const filename)
{
	if (fp==NULL) {
		perror(filename);
		exit(EXIT_FAILURE);
	}
}
void memError(void *p, const char * const errormsg)
{
	if (p==NULL) {
		fprintf(stderr, "\nmemory error: %s\n" , errormsg);
		exit(EXIT_FAILURE);
	}
}
void isError(const char * const errormsg)
{
	fprintf(stderr, "[ERROR]:\n\t%s.\n", errormsg);
	exit(EXIT_FAILURE);
}
/********************************************************************************************************/

/********************************************************************************************************/
double dmin(double a, double b) {
	return a<b?a:b;
}
double dmax(double a, double b) {
	return a>b?a:b;
}
int imin(int a, int b) {
	return a<b?a:b;
}
int imax(int a, int b) {
	return a>b?a:b;
}
long lmin(long a, long b) {
	return a<b?a:b;
}
long lmax(long a, long b) {
	return a>b?a:b;
}
/********************************************************************************************************/
