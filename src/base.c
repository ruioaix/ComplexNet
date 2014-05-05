#include <stdio.h>

/********************************************************************************************************/
#include <time.h>
void print_time(void) {
	time_t t=time(NULL); 
	printf("TIME =>> \t%s", ctime(&t)); 
	(void)fflush(stdout);
}
void print_label(int i) {
	printf("xxxx==%d==xxxx\n", i);
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

/********************************************************************************************************/
#include <assert.h>
void *smalloc(size_t size) {
	void *tmp = malloc(size);
	assert(tmp != NULL);
	return tmp;
}
void srealloc(void *p, size_t size) {
	void *tmp = realloc(p, size);
	assert(tmp != NULL);
	p = tmp;
}
/********************************************************************************************************/
