/**
 * This file may be included by any other *.c or *.h file.
 * it provide functions and preprocessor macros that could be used most commonly.
 *
 * Author: RuiXiao <xrfind@gmail.com>
 */

#ifndef CN_BASE_H
#define CN_BASE_H

/********************************************************************************************************/
#define NDEBUG
/********************************************************************************************************/

/********************************************************************************************************/
void print_time(void);
void print_label(int i);
/********************************************************************************************************/

/********************************************************************************************************/
#include <stdio.h> //for FILE, perror, fprintf, stderr
void fileError(FILE *fp, const char * const filename);
void memError(void *p, const char * const errormsg);
void isError(const char * const errormsg);
/********************************************************************************************************/

/********************************************************************************************************/
double dmin(double a, double b);
double dmax(double a, double b);
int imin(int a, int b);
int imax(int a, int b);
long lmin(long a, long b);
long lmax(long a, long b);
/********************************************************************************************************/


/********************************************************************************************************/
void *smalloc(size_t size);
void srealloc(void *p, size_t size);
/********************************************************************************************************/





#endif
