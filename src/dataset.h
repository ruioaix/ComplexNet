/**
 * this file is used to generate all kinds of data set.
 * the data set will be represented with struct LineFile or just print into file.
 */
#ifndef CN_DATASET_H
#define CN_DATASET_H

#include "linefile.h"

enum CICLENET {
	CYCLE, NON_CYCLE
};

enum DIRECTNET {
	DIRECT, NON_DIRECT
};


struct LineFile * lattice2d_DS(int L, enum CICLENET cc, enum DIRECTNET dd);
struct LineFile * line1d_DS(int L, enum CICLENET cc, enum DIRECTNET dd);

void parts45_DS(char *filename, long linesNum, int in, int dn, int cn, int ln, int ccn);

//ER random network
struct LineFile *ER_DS(int N, int seed);
//scare free network
struct LineFile *SF_DS(int N, int seed, int MM0);


#endif
