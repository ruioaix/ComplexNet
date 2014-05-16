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
#define VERBOSE_LEVEL_0 //no output, should be used when you can make sure the program is right, and what you want is only the result output.
//#define VERBOSE_LEVEL_1 //describe the logic of main.
//#define VERBOSE_LEVEL_2 //except above, additionally, describe all global function's calls.
//#define VERBOSE_LEVEL_3 //except above, additionally, describe all logic in each call of global function.
//#define VERBOSE_LEVEL_4 //except above, additionally, describe all static function's calls.
//#define VERBOSE_LEVEL_N //printing logic in loop!!! too much...
/*VERBOSE_LEVEL_0 means no-output, this only should be used when you can make sure the program is right,
VERBOSE_LEVEL_1 means only output the content of print1l. output the information of program running.
VERBOSE_LEVEL_2 means output the content of print1l&print2l. output somehow detail information.
VERBOSE_LEVEL_3 means output the content of print1l&print2l&print3l. output all information.
VERBOSE_LEVEL_N means output the content of print1l&print2l&print3l&printnl. 
	printnl often in loop, so the output will be in huge size.
later maybe we have print4l.*/

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
void *calloc_safe(size_t num, size_t size, const char *funcname, const char *filename, const int lineNum);
void *realloc_safe(void *p, size_t size, const char *funcname, const char *filename, const int lineNum);
#define smalloc(n) malloc_safe(n, __func__, __FILE__, __LINE__)
#define scalloc(n, s) calloc_safe(n, s, __func__, __FILE__, __LINE__)
#define srealloc(p, n) realloc_safe(p, n, __func__, __FILE__, __LINE__)
/********************************************************************************************************/

/********check some prerequisite*************************************************************************/
void prerequisite(void);
/********************************************************************************************************/

/********************************************************************************************************/
//print1l will be used in main function. and some special situation.
//print2l will be used in each global functions' begin and end position.
//print3l will be used to describ the logic in global function 
//print4l will be used in the static functions' begin&end position. but if static function is used in a loop, use printnl instead.
//print5l will be used in describe the logic in static function
//printnl will be used in loop.
#ifdef VERBOSE_LEVEL_0
#define print1l(format, ...) ((void)0)
#define print2l(format, ...) ((void)0)
#define print3l(format, ...) ((void)0)
#define print4l(format, ...) ((void)0)
#define print5l(format, ...) ((void)0)
#define printnl(format, ...) ((void)0)
#endif

#ifdef VERBOSE_LEVEL_1
#define print1l(format, ...) do {\
		printf("[level 1] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print2l(format, ...) ((void)0)
#define print3l(format, ...) ((void)0)
#define print4l(format, ...) ((void)0)
#define print5l(format, ...) ((void)0)
#define printnl(format, ...) ((void)0)
#endif

#ifdef VERBOSE_LEVEL_2
#define print1l(format, ...) do {\
		printf("[level 1] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print2l(format, ...) do {\
		printf("[level 2] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print3l(format, ...) ((void)0)
#define print4l(format, ...) ((void)0)
#define print5l(format, ...) ((void)0)
#define printnl(format, ...) ((void)0)
#endif

#ifdef VERBOSE_LEVEL_3
#define print1l(format, ...) do {\
		printf("[level 1] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print2l(format, ...) do {\
		printf("[level 2] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print3l(format, ...) do {\
		printf("[level 3] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print4l(format, ...) ((void)0)
#define print5l(format, ...) ((void)0)
#define printnl(format, ...) ((void)0)
#endif

#ifdef VERBOSE_LEVEL_4
#define print1l(format, ...) do {\
		printf("[level 1] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print2l(format, ...) do {\
		printf("[level 2] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print3l(format, ...) do {\
		printf("[level 3] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print4l(format, ...) do {\
		printf("[level 4] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print5l(format, ...) ((void)0)
#define printnl(format, ...) ((void)0)
#endif

#ifdef VERBOSE_LEVEL_5
#define print1l(format, ...) do {\
		printf("[level 1] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print2l(format, ...) do {\
		printf("[level 2] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print3l(format, ...) do {\
		printf("[level 3] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print4l(format, ...) do {\
		printf("[level 4] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print5l(format, ...) do {\
		printf("[level 4] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define printnl(format, ...) ((void)0)
#endif

#ifdef VERBOSE_LEVEL_N
#define print1l(format, ...) do {\
		printf("[level 1] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print2l(format, ...) do {\
		printf("[level 2] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print3l(format, ...) do {\
		printf("[level 3] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print4l(format, ...) do {\
		printf("[level 4] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define print5l(format, ...) do {\
		printf("[level 4] ");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#define printnl(format, ...) do {\
		printf("[level n] ");\
		printf("\t");\
		printf(format, ##__VA_ARGS__);\
} while(0)
#endif
/********************************************************************************************************/

#endif
