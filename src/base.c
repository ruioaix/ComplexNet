#include "base.h"

/********************************************************************************************************/
#include <time.h>
void print_time(void) {
	time_t t=time(NULL); 
	print1l("TIME =>> \t%s", ctime(&t)); 
	fflush(stdout);
}
void print_label(int i) {
	print1l("xxxx==%d==xxxx\n", i);
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

/********************************************************************************************************/
#include <limits.h>
#include <stdint.h>
void prerequisite(void) {
	print1l("prerequisite =>>\n\t");
	print1l("sizeof(int): %zd; INT_MAX: %d, 0x%X\n\t", sizeof(int), INT_MAX, (unsigned)INT_MAX);
	print1l("sizeof(long): %zd; LONG_MAC: %ld, 0x%lX\n\t", sizeof(long), LONG_MAX, (unsigned long)LONG_MAX);
	print1l("sizeof(size_t): %zd; SIZE_MAX: %zu, 0X%zX\n", sizeof(size_t), SIZE_MAX, SIZE_MAX);
	sizeof(int) < 4 ? isError("sizeof int too small"):1;
	sizeof(long) < 4 ? isError("sizeof long too small"):1;
	sizeof(size_t) < 4 || sizeof(size_t) < sizeof(int) ? isError("sizeof size_t too small"):1;
}
/********************************************************************************************************/

/********************************************************************************************************/
//#include <stdarg.h>
//void print1l(char *format, ...) {
//	va_list args;
//	if (VERBOSE_LEVEL<1)
//		return;
//
//	va_start(args, format);
//	vprintf(format, args);
//	va_end(args);
//}
//void print2l(char *format, ...) {
//	va_list args;
//	if (VERBOSE_LEVEL<2)
//		return;
//
//	va_start(args, format);
//	vprintf(format, args);
//	va_end(args);
//}
//void print3l(char *format, ...) {
//	va_list args;
//	if (VERBOSE_LEVEL<3)
//		return;
//
//	va_start(args, format);
//	vprintf(format, args);
//	va_end(args);
//}
/********************************************************************************************************/
