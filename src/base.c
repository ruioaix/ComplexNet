#include "base.h"

/********************************************************************************************************/
#include <time.h>
void print_time(void) {
	time_t t=time(NULL); 
	(void)t;
	print1l("time =>> %s", ctime(&t)); 
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
#include <stdarg.h>
void fileError(FILE *fp, char* format, ...) {
	if (fp==NULL) {
		fflush(stdout);
		va_list args;
		va_start(args, format);
		fprintf(stderr, "[ERROR]:\n\t");
		vfprintf(stderr, format, args);
		fprintf(stderr, "\n");
		va_end(args);
		exit(EXIT_FAILURE);
	}
}
void isError(char *format, ...) {
	fflush(stdout);
	va_list args;
	va_start(args, format);
	fprintf(stderr, "[ERROR]:\n\t");
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n\n");
	va_end(args);
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
void *malloc_safe(size_t size, const char *funcname, const char *filename, const int lineNum) {
	void *tmp = malloc(size);
	if (tmp == NULL) {
		isError("%s => malloc failed: \"%s\" file, %d line.\n", funcname, filename, lineNum);
	}
	return tmp;
}
void *calloc_safe(size_t num, size_t size, const char *funcname, const char *filename, const int lineNum) {
	void *tmp = calloc(num, size);
	if (tmp == NULL) {
		isError("%s => calloc failed: \"%s\" file, %d line.\n", funcname, filename, lineNum);
	}
	return tmp;
}
void *realloc_safe(void *p, size_t size, const char *funcname, const char *filename, const int lineNum) {
	void *tmp = realloc(p, size);
	if (tmp == NULL) {
		isError("%s => realloc failed: \"%s\" file, %d line.\n", funcname, filename, lineNum);
	}
	return tmp;
}
/********************************************************************************************************/

/********************************************************************************************************/
#include <limits.h>
#include <stdint.h>
void prerequisite(void) {
	print1l("%s =>> sizeof(int): %zd; INT_MAX: %d, 0x%X\n", __func__, sizeof(int), INT_MAX, (unsigned)INT_MAX);
	print1l("%s =>> sizeof(long): %zd; LONG_MAC: %ld, 0x%lX\n", __func__, sizeof(long), LONG_MAX, (unsigned long)LONG_MAX);
	print1l("%s =>> sizeof(size_t): %zd; SIZE_MAX: %zu, 0x%zX\n", __func__, sizeof(size_t), SIZE_MAX, SIZE_MAX);
	sizeof(int) < 4 ? isError("%s =>> sizeof int too small.\n", __func__):1;
	sizeof(long) < 4 ? isError("%s =>> sizeof long too small.\n", __func__):1;
	sizeof(size_t) < 4 || sizeof(size_t) < sizeof(int) ? isError("%s =>> sizeof size_t too small.\n", __func__):1;
}
/********************************************************************************************************/
