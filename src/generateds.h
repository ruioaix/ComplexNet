#ifndef CN_GENERATENET_H
#define CN_GENERATENET_H

#include "iilinefile.h"

enum CICLENET {
	cycle, non_cycle
};

enum DIRECTNET {
	direct, non_direct
};


struct iiLineFile * generate_2DLattice(int L, enum CICLENET cc, enum DIRECTNET dd);
struct iiLineFile * generate_1DLine(int L, enum CICLENET cc, enum DIRECTNET dd);




#endif
