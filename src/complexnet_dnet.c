#include "../inc/complexnet_dnet.h"
#include "../inc/complexnet_error.h"
#include "../inc/complexnet_threadpool.h"
#include "../inc/complexnet_random.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

static struct DirectNet dnet;

struct DirectNet *getDirectNet(void) {
	return &dnet;
}

void freeDNet(void) {
	int i=0;
	for(i=0; i<dnet.maxId+1; ++i) {
		if (dnet.count[i]>0) {
			free(dnet.to[i]);
		}
	}
	free(dnet.count);
	free(dnet.to);
}

void buildDNet(const struct iiLineFile * const file) {
	int maxId=file->iMax;
	int minId=file->iMin;
	long linesNum=file->linesNum;
	struct iiLine *lines=file->lines;

	long *count=calloc(maxId+1, sizeof(long));
	char *status=calloc(maxId+1, sizeof(char));
	assert(count!=NULL);
	assert(status!=NULL);

	long *temp_count=calloc(maxId+1, sizeof(long));
	assert(temp_count!=NULL);

	long i;
	for(i=0; i<linesNum; ++i) {
#ifdef VEXTER_FILE_DIRECTION_LEFT
		++count[lines[i].i2];
#else
		++count[lines[i].i1];
#endif
		++temp_count[lines[i].i2];
		++temp_count[lines[i].i1];
	}
	int j;
	int vtsNum=0;
	for(j=0; j<maxId+1; ++j) {
		if (temp_count[j]>0) {
			++vtsNum;
		}
	}

	int **to=calloc(maxId+1, sizeof(void *));
	assert(to!=NULL);
	long countMax=0;
	for(i=0; i<maxId+1; ++i) {
		if (countMax<count[i]) {
			countMax=count[i];
		}
		if (count[i]!=0) {
			to[i]=malloc(count[i]*sizeof(int));
			assert(to[i]!=NULL);
		}
	}

	memset(temp_count, 0, (maxId+1)*sizeof(long));
	for(i=0; i<linesNum; ++i) {
#ifdef VEXTER_FILE_DIRECTION_LEFT
		int id_from=lines[i].i2;
		int id_to=lines[i].i1;
#else
		int id_from=lines[i].i1;
		int id_to=lines[i].i2;
#endif
		to[id_from][temp_count[id_from]]=id_to;
		++temp_count[id_from];
	}
	free(temp_count);

	//struct DirectNet *dnet=malloc(sizeof(struct DirectNet));
	dnet.maxId=maxId;
	dnet.minId=minId;
	dnet.edgesNum=linesNum;
	dnet.vtsNum=vtsNum;
	dnet.countMax=countMax;
	dnet.count=count;
	dnet.to=to;
	printf("build direct net:\n\tMax: %d, Min: %d, vtsNum: %d, edgesNum: %ld, countMax: %ld\n", maxId, minId, vtsNum, linesNum, countMax); fflush(stdout);

}

int buildISStatusStick(const struct innLine * const is, char *statusStick) {
	assert(statusStick != NULL);

	int i=0;
	int sign=0;
	for (i=0; i<is->num; ++i) {
		int isvt=is->inn[i];
		int count=dnet.count[isvt];
		if (isvt > dnet.maxId || count == 0) {
			printf("IS Group %d:\tInfectSource %d is not existed in the net, ignored this IS Group.\n", is->lineId, isvt);fflush(stdout);
			return -1;
		}
		if (statusStick[isvt] == 1) {
			printf("IS Group %d:\tInfectSource %d duplicate, ignored this IS Group.\n", is->lineId, isvt);fflush(stdout);
			return -2;
		}
		statusStick[isvt] = 1;
		++sign;
	}
	if (!sign) {
		printf("IS Group %d:\tno valid is, ignored this IS Group.\n", is->lineId);fflush(stdout);
		return -3;
	}
	return 0;
}

//0S,1I,2R
// simple IS, clean dNet. 
//int dnet_spread_core(const struct InfectSource * const IS, const struct DirectNet * const dNet_origin, const double infectRate, const double touchParam, const int loopNum)
void *dnet_spread(void *args_void)
{
	struct DNetSpreadArgs *args = args_void;
	struct innLine *IS = args->IS;
	double infectRate = args->infectRate;
	double touchParam = args->touchParam;
	int loopNum = args->loopNum;

	unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
	int isId = init_by_array_MersenneTwister_threadsafe(init, length);

	char *statusStick = calloc(dnet.maxId+1, sizeof(char));
	assert(statusStick != NULL);

	
	if (buildISStatusStick(IS, statusStick)<0) {
		return (void *)-1;
	}

	int *oVt=malloc((dnet.maxId+1)*sizeof(int));
	assert(oVt!=NULL);
	int oNum;

	int *xVt=malloc((dnet.maxId+1)*sizeof(int));
	assert(xVt!=NULL);
	int xNum;

	double IR[loopNum];
	int l=0;
	int spreadStep=0;

	//char ttt[35];
	//sprintf(ttt, "isId%d", isId);
	//FILE *fp = fopen(ttt,"w");
	//int ii;
	//for (ii=0; ii<1000; ii++) {
	//	fprintf(fp, "%10lu ", genrand_int32_threadsafe(isId));
	//	if (ii%5==4) fprintf(fp, "\n");
	//}
	//fprintf(fp, "\n1000 outputs of genrand_real2()\n");
	//for (ii=0; ii<1000; ii++) {
	//	fprintf(fp, "%10.8f ", genrand_real2_threadsafe(isId));
	//	if (ii%5==4) fprintf(fp, "\n");
	//}
	//return (void *)0;

	for( l=0; l<loopNum; ++l) {	

		memset(statusStick, 0, (dnet.maxId+1)*sizeof(char));
		buildISStatusStick(IS, statusStick);

		memcpy(oVt, IS->inn, IS->num*sizeof(int));
		oNum=IS->num;

		while(oNum>0) {
			xNum=0;
			int i, j, neigh;
			double r, touchRate;
			//begin to try to spread.
			for (i=0; i<oNum; ++i) {
				int vt=oVt[i];
				touchRate=1/(double)pow(dnet.count[vt], touchParam);
				//I begin to spread to its neighbor
				for (j=0; j<dnet.count[vt]; ++j) {
					neigh=dnet.to[vt][j];
					//only S neighbour need to try. if it's I/R, nothing to do.
					r=genrand_real1_threadsafe(isId);
					//r=0.5;
					if (r<touchRate) {
						if (statusStick[neigh] == 0) {
							r=genrand_real1_threadsafe(isId);
							//r=0.5;
							if (r<infectRate) {
								statusStick[neigh] = 1;
								xVt[xNum++]=neigh;
							}
						}
					}
				}
				statusStick[vt] = 2;
			}

			int *temp = oVt; oVt = xVt; xVt = temp;
			oNum=xNum;
			++spreadStep;
		}

		int k, R=0;
		for (k=0; k<dnet.maxId+1; ++k) {
			if (statusStick[k]==2) {
				++R;
			}
		}
		IR[l]=(double)R/(double)dnet.vtsNum;

	}
	//printf("\t%d: just after loopNum----------------------\n", IS->ISId);fflush(stdout);
	double sp=0, s2p=0;
	for (l=0; l<loopNum; ++l) {
		sp+=IR[l]/loopNum;
		s2p+=IR[l]*IR[l]/loopNum;
	}
	double result=pow((s2p-pow(sp, 2))/(loopNum-1), 0.5);

	free(xVt);
	free(oVt);
	
	printf("IS Group %d:\tinfectRate:%f\tsp:%f\ts2p:%f\tfc:%f\tspreadStep:%f\n", IS->lineId, infectRate, sp, s2p, result, (double)spreadStep/(double)loopNum);fflush(stdout);
	return (void *)0;
}

struct DirectNet *cloneDNet(const struct DirectNet * const dnet) {
	struct DirectNet *dnet_c = malloc(sizeof(struct DirectNet));
	assert(dnet!=NULL && dnet_c!=NULL);

	dnet_c->maxId = dnet->maxId;
	dnet_c->minId = dnet->minId;
	dnet_c->countMax = dnet->countMax;
	dnet_c->edgesNum = dnet->edgesNum;
	dnet_c->vtsNum = dnet->vtsNum;

	dnet_c->count = malloc((dnet_c->maxId+1)*sizeof(long));
	assert(dnet_c->count != NULL);
	memcpy(dnet_c->count, dnet->count, (dnet_c->maxId+1)*sizeof(long));

	dnet_c->to = calloc(dnet_c->maxId+1, sizeof(void *));
	assert(dnet_c->to !=NULL);
	int i=0;
	for (i=0; i<dnet_c->maxId+1; ++i) {
		if(dnet->count[i] > 0) {
			dnet_c->to[i] = malloc(dnet_c->count[i]*sizeof(int));
			assert(dnet_c->to[i]);
			memcpy(dnet_c->to[i], dnet->to[i], dnet_c->count[i]*sizeof(int));
		}
	}
	return dnet_c;
}

void *verifyDNet(void *arg) {
	//struct DirectNet *dnet = arg;
	long i;
	int j;
	int *place = malloc((dnet.maxId+1)*sizeof(int));
	memset(place, -1, dnet.maxId+1);
	FILE *fp = fopen("data/duplicatePairsinDirectNet", "w");
	fileError(fp, "data/duplicatePairsinDirectNet");
	FILE *fp2 = fopen("data/NoDuplicatePairsNetFile", "w");
	fileError(fp2, "data/NoDuplicatePairsNetFile");
	fprintf(fp, "the following pairs are duplicate in the net file\n");
	char sign=0;
	for (j=0; j<dnet.maxId; ++j) {
		if (dnet.count[j]>0) {
			memset(place, -1, (dnet.maxId+1)*sizeof(int));
			for (i=0; i<dnet.count[j]; ++i) {
				int origin = dnet.to[j][i];
				int next = place[origin];
				if (next == -1) {
					place[origin]=origin;
					fprintf(fp2, "%d\t%d\n", j,origin);
				}
				else {
					fprintf(fp, "%d\t%d\n", j, next);
					sign=1;
				}
			}
		}
	}
	free(place);
	fclose(fp);
	fclose(fp2);
	if (sign == 1) {
		isError("the file has duplicate pairs, you can check data/duplicatePairsinDirectNet.\nwe generate a net file named data/NoDuplicatePairsNetFile which doesn't contain any duplicate pairsr.\nyou should use this file instead the origin wrong one.\n");
	}
	else {
		printf("verifyDNet: perfect network.\n");
	}
	return (void *)0;
}
