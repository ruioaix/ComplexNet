#ifndef COMPLEXNET_ERROR_H
#define COMPLEXNET_ERROR_H

#include <stdio.h> //for FILE, perror, fprintf, stderr
void fileError(FILE *fp, const char * const filename);
void memError(void *p, char *errormsg);
void isError(const char * const errormsg);

#endif
