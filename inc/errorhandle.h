#ifndef ERRORHANDLE_H
#define ERRORHANDLE_H

//for NULL
#include <stddef.h> 

//for perror, fprintf, stderr
#include <stdio.h>

//for exit & EXIT_FAILURE
#include <stdlib.h>

void fileError(FILE *fp, char *filename);
void memError(void *p, char *errormsg);
void isError(char *errormsg);

#endif
