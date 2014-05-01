#ifndef CN_COMMON_H
#define CN_COMMON_H

/********************************************************************************************************/
#define NDEBUG
/********************************************************************************************************/

/********************************************************************************************************/
void print_time(void);
void set_RandomSeed(void);
/********************************************************************************************************/

/********************************************************************************************************/
#include <stdio.h> //for FILE, perror, fprintf, stderr
void fileError(FILE *fp, const char * const filename);
void memError(void *p, const char * const errormsg);
void isError(const char * const errormsg);
/********************************************************************************************************/

#endif