/**
 * This file may be included by any other *.c or *.h file.
 * it provide functions and preprocessor macros that could be used most commonly.
 *
 * Author: RuiXiao <xrfind@gmail.com>
 */

#ifndef CN_BASE_H
#define CN_BASE_H

/********************************************************************************************************/
//#define NDEBUG
#include <assert.h>
/********************************************************************************************************/

/********************************************************************************************************/
void print_time(void);
void print_label(int i);
/********************************************************************************************************/

/********************************************************************************************************/
#include <stdio.h> //for FILE, perror, fprintf, stderr
void fileError(FILE *fp, const char * const filename);
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

/********************************************************************************************************/
void prerequisite(void);
/********************************************************************************************************/

/********************************************************************************************************/
#define VERBOSE_LEVEL_2
//VERBOSE_LEVEL_0 means no-output, this only should be used when you can make sure the program is right,
//	and what you want is only the result output.
//VERBOSE_LEVEL_1 means only output the content of print1l. output the information of program running.
//VERBOSE_LEVEL_2 means output the content of print1l&print2l. output somehow detail information.
//VERBOSE_LEVEL_3 means output the content of print1l&print2l&print3l. output all information.
//later maybe we have print4l.

#ifdef VERBOSE_LEVEL_0
#define print1l(format, ...) ((void)0)
#define print2l(format, ...) ((void)0)
#define print3l(format, ...) ((void)0)
#endif

#ifdef VERBOSE_LEVEL_1
#define print1l(format, ...) do {\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print2l(format, ...) ((void)0)
#define print3l(format, ...) ((void)0)
#endif

#ifdef VERBOSE_LEVEL_2
#define print1l(format, ...) do {\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print2l(format, ...) do {\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print3l(format, ...) ((void)0)
#endif

#ifdef VERBOSE_LEVEL_3
#define print1l(format, ...) do {\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print2l(format, ...) do {\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print3l(format, ...) do {\
		printf(format, ##__VA_ARGS__);\
} while(0)
#endif


//#define print1l(format, ...) (void *

//void print1l(char *format, ...);
//void print2l(char *format, ...);
//void print3l(char *format, ...);
/********************************************************************************************************/

#endif
