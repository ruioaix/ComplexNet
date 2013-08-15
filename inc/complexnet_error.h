#ifndef COMPLEXNET_ERROR_H
#define COMPLEXNET_ERROR_H

//for NULL
#include <stddef.h> 

//for perror, fprintf, stderr
#include <stdio.h>

//for exit & EXIT_FAILURE
#include <stdlib.h>

void fileError(FILE *fp, const char * const filename);
void memError(void *p, char *errormsg);
void isError(const char * const errormsg);

#endif
