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
extern void print_time(void);
/********************************************************************************************************/

/********************************************************************************************************/
#include <stdio.h> //for FILE, perror, fprintf, stderr
extern void fileError(FILE *fp, const char * const filename);
extern void memError(void *p, const char * const errormsg);
extern void isError(const char * const errormsg);
/********************************************************************************************************/

void print_label(void);

#endif
