#include "../inc/complexnet_dnet.h"

void freeDNet(struct DirectNet *dnet) {
	free(dnet->status);
	vttype i=0;
	for(i=0; i<dnet->maxId+1; ++i) {
		if (dnet->count[i]>0) {
			free(dnet->to[i]);
		}
	}
	free(dnet->count);
	free(dnet->to);
	free(dnet);
}

struct DirectNet *buildDNet(struct NetFile *file) {
	vttype maxId=file->maxId;
	vttype minId=file->minId;
	edtype linesNum=file->linesNum;
	struct LineInfo *lines=file->lines;

	edtype *count=calloc(maxId+1, sizeof(edtype));
	char *status=calloc(maxId+1, sizeof(char));
	assert(count!=NULL);
	assert(status!=NULL);

	edtype i;
	for(i=0; i<linesNum; ++i) {
#ifdef VEXTER_FILE_DIRECTION_LEFT
		++count[lines[i].vt2Id];
#else
		++count[lines[i].vt1Id];
#endif
	}
	vttype j;
	vttype vtsNum=0;
	for(j=0; j<maxId+1; ++j) {
		if (count[j]>0) {
			++vtsNum;
		}
	}

	vttype **to=calloc(maxId+1, sizeof(void *));
	assert(to!=NULL);
	edtype countMax=0;
	for(i=0; i<maxId+1; ++i) {
		if (countMax<count[i]) {
			countMax=count[i];
		}
		if (count[i]!=0) {
			to[i]=malloc(count[i]*sizeof(vttype));
			assert(to[i]!=NULL);
		}
	}

	edtype *temp_count=calloc(maxId+1, sizeof(edtype));
	assert(temp_count!=NULL);
	for(i=0; i<linesNum; ++i) {
#ifdef VEXTER_FILE_DIRECTION_LEFT
		vttype id_from=lines[i].vt2Id;
		vttype id_to=lines[i].vt1Id;
#else
		vttype id_from=lines[i].vt1Id;
		vttype id_to=lines[i].vt2Id;
#endif
		to[id_from][temp_count[id_from]]=id_to;
		++temp_count[id_from];
	}
	free(temp_count);

	struct DirectNet *dnet=malloc(sizeof(struct DirectNet));
	dnet->maxId=maxId;
	dnet->minId=minId;
	dnet->edgesNum=linesNum;
	dnet->vtsNum=vtsNum;
	dnet->countMax=countMax;
	dnet->count=count;
	dnet->status=status;
	dnet->to=to;
	printf("build direct net:\n\tMax: %d, Min: %d, vtsNum: %d, edgesNum: %d, countMax: %d\n", maxId, minId, vtsNum, linesNum, countMax); fflush(stdout);

	return dnet;
}

void buildIStoDNet(struct InfectSource *is, struct DirectNet *dnet) {
	vttype i=0;
	int sign=0;
	for (i=0; i<is->num; ++i) {
		vttype isvt=is->vt[i];
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
// simple IS, clean dNet. just one spread.
int dnet_spread_core(struct InfectSource *IS, struct DirectNet *dNet, double infectRate, double touchParam)
{
	buildIStoDNet(IS, dNet);

	vttype *oVt=malloc(dNet->maxId*sizeof(vttype));
	assert(IS->num<=dNet->maxId);
	memcpy(oVt, IS->vt, IS->num*sizeof(vttype));
	vttype oNum=IS->num;

	vttype *xVt=malloc(dNet->maxId*sizeof(vttype));
	vttype xNum;

	int spreadStep=0;
	while(oNum>0) {
		xNum=0;
		vttype i, j, neigh;
		double r, touchRate;
		//begin to try to spread.
		for (i=0; i<oNum; ++i) {
			vttype vt=oVt[i];
			touchRate=1/(double)pow(dNet->count[vt], touchParam);
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

		vttype *temp = oVt;
		oVt = xVt;
		xVt = temp;
		oNum=xNum;
		++spreadStep;
	}

	free(xVt);
	free(oVt);
	return spreadStep;
}

int dnet_spread(struct InfectSource *IS, struct DirectNet *dNet, double infectRate, double touchParam, int loopNum) {
	printf("begin to spread:\n");
	struct InfectSource *is = malloc(sizeof(struct InfectSource));
	assert(is!=NULL);
	is->num = 1;
	is->vt = malloc(sizeof(vttype));
	assert(is->vt!=NULL);

	double IR[loopNum];

	vttype i=0, k=0, R=0;
	int j=0;
	int spreadstepsNum=0;
	struct DirectNet *dNet_c = cloneDNet(dNet);
	for (i=0; i<IS->num; ++i) {
		is->vt[0] = IS->vt[i];
		spreadstepsNum=0;
		for( j=0; j<loopNum; ++j) {	
			R=0;
			//cloneDNet(dNet_c, dNet);
			memset(dNet_c->status, 0, (dNet_c->maxId+1)*sizeof(char));
			spreadstepsNum += dnet_spread_core(is, dNet_c, infectRate, touchParam);
			for (k=0; k<dNet_c->maxId+1; ++k) {
				if (dNet_c->status[k]==2) {
					++R;
				}
			}
			IR[j]=(double)R/(double)dNet_c->vtsNum;
		}
	
		double sp=0, s2p=0;
		for (j=0; j<loopNum; ++j) {
			sp+=IR[j]/loopNum;
			s2p+=IR[j]*IR[j]/loopNum;
		}
	
		double result=pow((s2p-pow(sp, 2))/(loopNum-1), 0.5);
		printf("%d\tinfectRate:%f\tsp:%f\ts2p:%f\tfc:%f\tspreadStep:%f\n", i, infectRate, sp, s2p, result, (double)spreadstepsNum/(double)loopNum);
	}
	freeDNet(dNet_c);
	free(is->vt);
	free(is);
	return 0;
}

struct DirectNet *cloneDNet(struct DirectNet *dnet) {
	struct DirectNet *dnet_c = malloc(sizeof(struct DirectNet));
	assert(dnet!=NULL && dnet_c!=NULL);

	dnet_c->maxId = dnet->maxId;
	dnet_c->minId = dnet->minId;
	dnet_c->countMax = dnet->countMax;
	dnet_c->edgesNum = dnet->edgesNum;
	dnet_c->vtsNum = dnet->vtsNum;

	dnet_c->count = malloc((dnet_c->maxId+1)*sizeof(edtype));
	assert(dnet_c->count != NULL);
	memcpy(dnet_c->count, dnet->count, (dnet_c->maxId+1)*sizeof(edtype));

	dnet_c->status = malloc((dnet_c->maxId+1)*sizeof(char));
	assert(dnet_c->status != NULL);
	memcpy(dnet_c->status, dnet->status, (dnet_c->maxId+1)*sizeof(char));

	dnet_c->to = calloc(dnet_c->maxId+1, sizeof(void *));
	assert(dnet_c->to !=NULL);
	vttype i=0;
	for (i=0; i<dnet_c->maxId+1; ++i) {
		if(dnet->count[i] > 0) {
			dnet_c->to[i] = malloc(dnet_c->count[i]*sizeof(vttype));
			assert(dnet_c->to[i]);
			memcpy(dnet_c->to[i], dnet->to[i], dnet_c->count[i]*sizeof(vttype));
		}
	}
	return dnet_c;
}
