/**
 * This file may be included by any other *.c or *.h file.
 * it provide functions and preprocessor macros that could be used most commonly.
 *
 * Author: RuiXiao <xrfind@gmail.com>
 */
#ifndef CN_BASE_H
#define CN_BASE_H
/****************Configuration Section*******************************************************************/
//#define NDEBUG
//#define VERBOSE_LEVEL_0
//#define VERBOSE_LEVEL_1
//#define VERBOSE_LEVEL_2
#define VERBOSE_LEVEL_3
//#define VERBOSE_LEVEL_N

/********************************************************************************************************/

/********assert should be used very offen.***************************************************************/
#include <assert.h>
/********************************************************************************************************/

/********use everywhere.*********************************************************************************/
void print_time(void);
void print_label(int i);
/********************************************************************************************************/

/*******use everywhere.**********************************************************************************/
#include <stdio.h> //for FILE, perror, fprintf, stderr
void fileError(FILE *fp, char* format, ...);
void isError(char *format, ...);
/********************************************************************************************************/

/*******some time, I use*********************************************************************************/
double dmin(double a, double b);
double dmax(double a, double b);
int imin(int a, int b);
int imax(int a, int b);
long lmin(long a, long b);
long lmax(long a, long b);
/********************************************************************************************************/

/*********use everywhere.********************************************************************************/
void *malloc_safe(size_t size, const char *funcname, const char *filename, const int lineNum);
void realloc_safe(void *p, size_t size, const char *funcname, const char *filename, const int lineNum);
#define smalloc(n) malloc_safe(n, __func__, __FILE__, __LINE__)
#define srealloc(p, n) realloc_safe(p, n, __func__, __FILE__, __LINE__)
/********************************************************************************************************/

/********check some prerequisite*************************************************************************/
void prerequisite(void);
/********************************************************************************************************/

/********************************************************************************************************/
/*VERBOSE_LEVEL_0 means no-output, this only should be used when you can make sure the program is right,
	and what you want is only the result output.
VERBOSE_LEVEL_1 means only output the content of print1l. output the information of program running.
VERBOSE_LEVEL_2 means output the content of print1l&print2l. output somehow detail information.
VERBOSE_LEVEL_3 means output the content of print1l&print2l&print3l. output all information.
VERBOSE_LEVEL_N means output the content of print1l&print2l&print3l&printnl. 
	printnl often in loop, so the output will be in huge size.
later maybe we have print4l.*/
#ifdef VERBOSE_LEVEL_0
#define print1l(format, ...) ((void)0)
#define print2l(format, ...) ((void)0)
#define print3l(format, ...) ((void)0)
#define printnl(format, ...) ((void)0)
#endif

#ifdef VERBOSE_LEVEL_1
#define print1l(format, ...) do {\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print2l(format, ...) ((void)0)
#define print3l(format, ...) ((void)0)
#define printnl(format, ...) ((void)0)
#endif

#ifdef VERBOSE_LEVEL_2
#define print1l(format, ...) do {\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print2l(format, ...) do {\
		printf("   ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print3l(format, ...) ((void)0)
#define printnl(format, ...) ((void)0)
#endif

#ifdef VERBOSE_LEVEL_3
#define print1l(format, ...) do {\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print2l(format, ...) do {\
		printf("   ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print3l(format, ...) do {\
		printf("      ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define printnl(format, ...) ((void)0)
#endif

#ifdef VERBOSE_LEVEL_N
#define print1l(format, ...) do {\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print2l(format, ...) do {\
		printf("   ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print3l(format, ...) do {\
		printf("      ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define printnl(format, ...) do {\
		printf("\t\t");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#endif
/********************************************************************************************************/

#endif
