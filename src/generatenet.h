#ifndef CN_GENERATENET_H
#define CN_GENERATENET_H

#include "iilinefile.h"

enum CICLENET {
	cycle, non_cycle
};

struct iiLineFile * generateNet_2D(int L, enum CICLENET cc);
struct iiLineFile * generateNet_1D(int L, enum CICLENET cc);




#endif
