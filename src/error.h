#ifndef CN_ERROR_H
#define CN_ERROR_H

#include <stdio.h> //for FILE, perror, fprintf, stderr
void fileError(FILE *fp, const char * const filename);
void memError(void *p, const char * const errormsg);
void isError(const char * const errormsg);

#endif
