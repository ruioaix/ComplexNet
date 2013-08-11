#include "../inc/complexnet_dnet.h"

struct DirectNet *buildDNet(struct NetFile *file) {
	idtype maxId=file->maxId;
	idtype minId=file->minId;
	linesnumtype linesNum=file->linesNum;
	struct LineInfo *lines=file->lines;

	linesnumtype *count=calloc(maxId+1, sizeof(linesnumtype));
	char *status=calloc(maxId+1, sizeof(char));
	assert(count!=NULL);
	assert(status!=NULL);

	linesnumtype i;
	for(i=0; i<linesNum; i++) {
		++count[lines[i].vt1Id];
	}
	
	idtype **to=calloc(maxId+1, sizeof(void *));
	assert(to!=NULL);
	linesnumtype countMax=0;
	for(i=0; i<maxId+1; i++) {
		if (countMax<count[i]) {
			countMax=count[i];
		}
		if (count[i]!=0) {
			to[i]=malloc(count[i]*sizeof(idtype));
			assert(to[i]!=NULL);
		}
	}

	linesnumtype *temp_count=calloc(maxId+1, sizeof(linesnumtype));
	assert(temp_count!=NULL);
	for(i=0; i<linesNum; i++) {
		idtype id_from=lines[i].vt1Id;
		idtype id_to=lines[i].vt2Id;
		to[id_from][temp_count[id_from]]=id_to;
		++temp_count[id_from];
	}
	free(temp_count);

	struct DirectNet *dnet=malloc(sizeof(struct DirectNet));
	dnet->maxId=maxId;
	dnet->minId=minId;
	dnet->edgesNum=linesNum;
	dnet->countMax=countMax;
	dnet->count=count;
	dnet->status=status;
	dnet->to=to;
	
	return dnet;
}

//1S,2I,3R
int spread_touch_all(struct InfectSource *IS, struct DirectNet *dNet, double infectRate)
{
	int spreadStep=0;
	int countE2=1000000;
	idtype *xVt=malloc(countE2*sizeof(idtype));
	while(IS->num>0) {
		idtype *oVt=IS->vt;
		idtype oNum=IS->num;

		idtype i, xNum=0;
		//judge how many vt need to be try spread.
		for (i=0; i<oNum; ++i) {
			xNum+=dNet->count[oVt[i]];
		}

		//if no vt need to be try, spread stop.
		if (xNum==0) {
			break;
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
		for (i=0; i<oNum; ++i) {
			idtype vt=oVt[i];
			//I begin to spread to its neighbor
			for (j=0; j<dNet->count[vt]; ++j) {
				neigh=dNet->to[vt][j];
				//only S neighbour need to try. if it's I/R, nothing to do.
				if (dNet->status[neigh] == 1) {
					r=genrand_real1();
					if (r<infectRate) {
						dNet->status[neigh] = 2;
						xVt[xNum++]=neigh;
					}
				}
			}
			dNet->status[vt] = 3;
		}
		
		idtype *temp = IS->vt;
		IS->vt = xVt;
		xVt = temp;
		IS->num=xNum;
	}
	free(IS->vt);
	free(xVt);
	++spreadStep;
	return spreadStep;
}
