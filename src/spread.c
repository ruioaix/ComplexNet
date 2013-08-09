#include "../inc/spread.h"
/*
void spread(struct Net *net, struct InfectSource *is, struct SpreadResult *SR)
{
	int spreadStep=0;
	double touchRate=is->touchRate;
	double infectRate=is->infectRate;
	SR->sNum=0;
	SR->iNum=0;
	SR->rNum=0;
	setIs2Net(net, is);
	while(is->isNum>0) {
		spreadStep++;
		int *lIs=is->is;
		int lNum=is->isNum;

		//1S,2I,3R
		int i, rNum=0;
		//judge how many vt need to be try spread.
		for (i=0; i<lNum; i++) {
			rNum+=net->count[lIs[i]];
		}

		//if no vt need to be try, spread stop.
		if (rNum==0) {
			free(is->is);
			is->is=NULL;
			is->isNum=0;
			break;
		}

		//get here means there are some vt need to be try.
		//get the space for store every vt need to be try.
		int *rIs=malloc(rNum*sizeof(int));
		rNum=0;
		int j, neigh;
		//begin to try to spread.
		for (i=0; i<lNum; i++) {
			int vt=lIs[i];
			//I begin to spread to its neighbor
			for (j=0; j<net->count[vt]; j++) {
				neigh=net->rela[vt][j];
				//I try to touch all neigh.
				double touchRandom=mt_drand();
				if (touchRandom<touchRate) {
					//only S neighbour can be tried to infect. if it's I/R, nothing to do.
					if (isCleanVT(net, neigh)) {
						//I try to infect S neigh.
						double infectRandom=mt_drand();
						if (infectRandom<infectRate) {
							setVTinfect(net, neigh);
							rIs[rNum++]=neigh;
						}
					}
				}
			}
			setVTdead(net, vt);
		}

		free(is->is);
		is->is=rIs;
		is->isNum=rNum;
	}
	getSIRnumofNet(net, SR);
	SR->spreadStep=spreadStep;
}

void setIs2Net(struct Net *net, struct InfectSource *is)
{
	int i;
	for (i=0; i<is->isNum; i++) {
		setVTinfect(net, is->is[i]);
	}
}

void getSIRnumofNet(struct Net *net, struct SpreadResult *SR)
{
	int i;
	for (i=0; i<net->vtNum; i++) {
		if (isCleanVT(net, i)) {
			SR->sNum++;
		}
		else if (isDeadVT(net, i)) {
			SR->rNum++;
		}
		else if (isInfectVT(net, i)) {
			SR->iNum++;
		}
	}
}
*/
