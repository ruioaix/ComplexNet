#ifndef SPREAD_H
#define SPREAD_H

#include "net.h"
#include "mtwist.h"

struct InfectSource {
	double infectRate;
	double touchRate;
	int *is;
	int isNum;
};

struct SpreadResult {
	int iNum;
	int sNum;
	int rNum;
	int spreadStep;
};

void spread(struct Net *net, struct InfectSource *is, struct SpreadResult *SR);
void setIs2Net(struct Net *net, struct InfectSource *is);
void getSIRnumofNet(struct Net *net, struct SpreadResult *SR);


#endif
