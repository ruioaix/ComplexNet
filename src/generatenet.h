#ifndef CN_GENERATENET_H
#define CN_GENERATENET_H

#include "iilinefile.h"

enum CICLENET {
	cycle, non_cycle
};

enum DIRECTNET {
	direct, non_direct
};

struct iiLineFile * generateNet_2D(int L, enum CICLENET cc, enum DIRECTNET dd);
struct iiLineFile * generateNet_1D(int L, enum CICLENET cc, enum DIRECTNET dd);




#endif
