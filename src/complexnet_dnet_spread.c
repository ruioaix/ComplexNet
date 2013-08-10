#include "../inc/complexnet_dnet.h"
#include "../inc/complexnet_dnet_spread.h"
#include "../inc/complexnet_random.h"


//1S,2I,3R
int spread_touch_all(struct InfectSource *IS, struct DirectNet *dNet, double infectRate)
{
	int spreadStep=0;
	int countE2=1000000;
	idtype *xVt=malloc(countE2*sizeof(idtype));
	while(IS->num>0) {
		++spreadStep;
		idtype *oVt=IS->vt;
		idtype oNum=IS->num;

		idtype i, xNum=0;
		//judge how many vt need to be try spread.
		for (i=0; i<oNum; ++i) {
			xNum+=dNet->count[oVt[i]];
		}

		//if no vt need to be try, spread stop.
		if (xNum==0) {
			free(IS->vt);
			IS->vt=NULL;
			IS->num=0;
			return spreadStep;
		}

		//get here means there are some vt need to be try.
		//get the space for store every vt need to be try.
		if (xNum>countE2) {
			idtype *temp=realloc(xVt, xNum*sizeof(idtype));
			if (temp!=NULL) xVt=temp;
			countE2 = xNum;
		}
		xNum=0;
		idtype j, neigh;
		double r;
		//begin to try to spread.
		for (i=0; i<oNum; i++) {
			idtype vt=oVt[i];
			//I begin to spread to its neighbor
			for (j=0; j<dNet->count[vt]; j++) {
				neigh=dNet->to[vt][j];
				//TODO 
				//only S neighbour need to try. if it's I/R, nothing to do.
				if (dNet->status[neigh] == 1) {
					//TODO
					r=genrand_real1();
					if (r<infectRate) {
						//setVTinfect(Net, neigh);
						dNet->status[neigh] = 2;
						xVt[xNum++]=neigh;
					}
				}
			}
			//setVTdead(Net, vt);
			dNet->status[vt] = 3;
		}

		free(IS->vt);
		IS->vt=xVt;
		IS->num=xNum;
	}
	return spreadStep;
}
