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
int spread_touch_all_core(struct InfectSource *IS, struct DirectNet *dNet, double infectRate)
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
int spread_touch_part_core(struct InfectSource *IS, struct DirectNet *dNet, double infectRate)
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


int spread_touch_all(struct InfectSource *IS, struct DirectNet *dNet, double infectRate) {
	struct InfectSource *is = malloc(sizeof(struct InfectSource));
	assert(is!=NULL);
	is->num = 1;
	is->vt = malloc(sizeof(idtype));
	assert(is->vt!=NULL);

	double IR[IS->num];

	idtype i=0;
	for (i=0; i<IS->num; ++i) {
		is->vt[0] = IS->vt[i];
		struct DirectNet *dNet_c = cloneDNet(dNet);
		spread_touch_all_core(is, dNet_c, infectRate);

		idtype j=0;
		idtype R=0;
		for (j=0; j<dNet_c->maxId+1; ++j) {
			if (dNet_c->status[j] == 2) {
				++R;
			}
		}
		IR[i]=(double)R/(double)dNet_c->vtRealNum;

		freeDNet(dNet_c);
	}
}

void printDNetInfor(struct DirectNet *dnet) {
	int R=0;
	for (i=0; i<Net->vtMax+1; i++) {
		if (isDeadVT(Net, i)) {
			R++;
		}
	}
	IR[j]=(double)R/(double)Net->vtRealNum;

	double sp=0, s2p=0;
	for (i=0; i<loopNum; i++) {
		sp+=IR[i]/loopNum;
		s2p+=IR[i]*IR[i]/loopNum;
	}

	double result=pow((s2p-pow(sp, 2))/(loopNum-1), 0.5);
	printf("%d\tinfectRate:%f\tsp:%f\ts2p:%f\tfc:%f\tspreadStep:%d\n", k, infectRate, sp, s2p, result, spreadStep);

}

struct DirectNet *cloneDNet(struct DirectNet *dnet) {
	assert(dnet!=NULL);

	struct DirectNet *dnet_c = malloc(sizeof(struct DirectNet));
	assert(dnet_c != NULL);

	dnet_c = dnet;
	dnet_c->count = malloc((dnet_c->maxId+1)*sizeof(linesnumtype));
	assert(dnet_c->count != NULL);
	memcpy(dnet_c->count, dnet->count, (dnet_c->maxId+1)*sizeof(linesnumtype));

	dnet_c->status = malloc((dnet_c->maxId+1)*sizeof(char));
	assert(dnet_c->status != NULL);
	memcpy(dnet_c->status, dnet->status, (dnet_c->maxId+1)*sizeof(char));

	dnet_c->to = malloc((dnet_c->maxId+1)*sizeof(void *));
	assert(dnet_c->to !=NULL);
	memcpy(dnet_c->to, dnet->to, (dnet_c->maxId+1)*sizeof(void *));

	idtype i=0;
	for (i=0; i<dnet_c->maxId+1; ++i) {
		if(dnet_c->count[i] > 0) {
			dnet_c->to[i] = malloc(dnet_c->count[i]*sizeof(idtype));
			assert(dnet_c->to[i]);
			memcpy(dnet_c->to[i], dnet->to[i], dnet_c->count[i]*sizeof(idtype));
		}
	}

	return dnet_c;
}
