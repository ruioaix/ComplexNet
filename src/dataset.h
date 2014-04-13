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




#endif
