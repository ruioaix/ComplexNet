#include "../inc/complexnet_dnet.h"

void freeDNet(struct DirectNet *dnet) {
	free(dnet->count);
	free(dnet->status);
	idtype i=0;
	for(i=0; i<dnet->maxId+1; ++i) {
		free(dnet->to[i]);
	}
	free(dnet->to);
	free(dnet);
}

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

void buildIStoDNet(struct InfectSource *is, struct DirectNet *dnet) {
	idtype i=0;
	int sign=0;
	for (i=0; i<is->num; ++i) {
		idtype isvt=is->vt[i];
		if (isvt > dnet->maxId) {
			printf("InfectSource %d is not existed in the net, ignored.\n", isvt);
			continue;
		}
		dnet->status[isvt] = 1;
		++sign;
	}
	if (!sign) {
		printf("no InfectSource existed in the net.");
		exit(-1);
	}
}

//0S,1I,2R
int spread_touch_all(struct InfectSource *IS, struct DirectNet *dNet, double infectRate)
{
	buildIStoDNet(IS, dNet);
	int spreadStep=0;
	int countE2=1000000;
	if (IS->num > countE2) {
		countE2 = IS->num;
	}
	idtype *oVt=malloc(countE2*sizeof(idtype));
	idtype *xVt=malloc(countE2*sizeof(idtype));
	memcpy(oVt, IS->vt, IS->num*sizeof(idtype));
	idtype oNum=IS->num;
	idtype xNum;

	while(oNum>0) {
		
		xNum=0;
		idtype i;
		//judge how many vt need to be try spread.
		for (i=0; i<oNum; ++i) {
			xNum+=dNet->count[oVt[i]];
		}
		//printf("xNum: %d\n", xNum);
		if (xNum==0) break; 
		if (xNum>countE2) {
			free(xVt);
			xVt=malloc(xNum*sizeof(idtype));
			//idtype *temp=realloc(xVt, xNum*sizeof(idtype));
			//assert(temp!=NULL);
			//if (temp!=NULL) xVt=temp;
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
				if (dNet->status[neigh] == 0) {
					r=genrand_real1();
					if (r<infectRate) {
						dNet->status[neigh] = 1;
						xVt[xNum++]=neigh;
					}
				}
			}
			dNet->status[vt] = 2;
		}
		
		idtype *temp = oVt;
		oVt = xVt;
		xVt = temp;
		oNum=xNum;
		++spreadStep;
	}

	free(xVt);
	free(oVt);
	return spreadStep;
}

//0S,1I,2R
int spread_touch_part(struct InfectSource *IS, struct DirectNet *dNet, double infectRate)
{
	int spreadStep=0;
	int countE2=1000000;
	if (IS->num > countE2) {
		countE2 = IS->num;
	}
	idtype *oVt=malloc(countE2*sizeof(idtype));
	idtype *xVt=malloc(countE2*sizeof(idtype));
	memcpy(oVt, IS->vt, IS->num*sizeof(idtype));
	free(IS->vt);
	idtype oNum=IS->num;
	idtype xNum;

	while(oNum>0) {
		
		xNum=0;
		idtype i;
		//judge how many vt need to be try spread.
		for (i=0; i<oNum; ++i) {
			xNum+=dNet->count[oVt[i]];
		}
		//printf("xNum: %d\n", xNum);
		if (xNum==0) break; 
		if (xNum>countE2) {
			free(xVt);
			xVt=malloc(xNum*sizeof(idtype));
			//idtype *temp=realloc(xVt, xNum*sizeof(idtype));
			//assert(temp!=NULL);
			//if (temp!=NULL) xVt=temp;
			countE2 = xNum;
		}


		xNum=0;
		idtype j, neigh;
		double r, touchRate;
		//begin to try to spread.
		for (i=0; i<oNum; ++i) {
			idtype vt=oVt[i];
			touchRate=1/(double)pow(dNet->count[vt], 0.5);
			//I begin to spread to its neighbor
			for (j=0; j<dNet->count[vt]; ++j) {
				neigh=dNet->to[vt][j];
				//only S neighbour need to try. if it's I/R, nothing to do.
				r=genrand_real1();
				if (r<touchRate) {
					if (dNet->status[neigh] == 0) {
						r=genrand_real1();
						if (r<infectRate) {
							dNet->status[neigh] = 1;
							xVt[xNum++]=neigh;
						}
					}
				}
			}
			dNet->status[vt] = 2;
		}
		
		idtype *temp = oVt;
		oVt = xVt;
		xVt = temp;
		oNum=xNum;
		++spreadStep;
	}

	free(xVt);
	free(oVt);
	return spreadStep;
}
