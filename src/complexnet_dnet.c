#include "../inc/complexnet_dnet.h"
#include "../inc/complexnet_threadpool.h"

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

struct DirectNet *buildDNet(const struct NetFile * const file) {
	vttype maxId=file->maxId;
	vttype minId=file->minId;
	edtype linesNum=file->linesNum;
	struct NetLineInfo *lines=file->lines;

	edtype *count=calloc(maxId+1, sizeof(edtype));
	char *status=calloc(maxId+1, sizeof(char));
	assert(count!=NULL);
	assert(status!=NULL);

	edtype *temp_count=calloc(maxId+1, sizeof(edtype));
	assert(temp_count!=NULL);

	edtype i;
	for(i=0; i<linesNum; ++i) {
#ifdef VEXTER_FILE_DIRECTION_LEFT
		++count[lines[i].vt2Id];
#else
		++count[lines[i].vt1Id];
#endif
		++temp_count[lines[i].vt2Id];
		++temp_count[lines[i].vt1Id];
	}
	vttype j;
	vttype vtsNum=0;
	for(j=0; j<maxId+1; ++j) {
		if (temp_count[j]>0) {
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

	memset(temp_count, 0, (maxId+1)*sizeof(edtype));
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

int buildIStoDNet(const struct InfectSource * const is, struct DirectNet *dnet) {
	vttype i=0;
	int sign=0;
	for (i=0; i<is->num; ++i) {
		vttype isvt=is->vt[i];
		vttype count=dnet->count[isvt];
		if (isvt > dnet->maxId || count == 0) {
			printf("IS Group %d:\tInfectSource %d is not existed in the net, ignored this IS Group.\n", is->ISId, isvt);fflush(stdout);
			return -2;
		}
		if (dnet->status[isvt] == 1) {
			printf("IS Group %d:\tInfectSource %d duplicate, ignored this IS Group.\n", is->ISId, isvt);fflush(stdout);
			return -3;
		}
		dnet->status[isvt] = 1;
		++sign;
	}
	if (!sign) {
		printf("IS Group %d:\tno valid is, ignored this IS Group.\n", is->ISId);fflush(stdout);
		return -1;
	}
	return 0;
}

//0S,1I,2R
// simple IS, clean dNet. 
//int dnet_spread_core(const struct InfectSource * const IS, const struct DirectNet * const dNet_origin, const double infectRate, const double touchParam, const int loopNum)
void *dnet_spread_core(void *args_void)
{
	struct DNetSpreadCoreArgs *args = args_void;
	struct DirectNet *dNet_origin = args->dNet;
	struct InfectSource *IS = args->IS;
	double infectRate = args->infectRate;
	double touchParam = args->touchParam;
	int loopNum = args->loopNum;
	vttype isId=args->isId;

	//printf("thread %d begin: ", isId);fflush(stdout);

	unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
	isId = init_by_array_MersenneTwister_threadsafe(init, length);

	struct DirectNet *dNet = cloneDNet(dNet_origin);
	if (buildIStoDNet(IS, dNet)< 0) {
		freeDNet(dNet);
		return (void *)-1;
	}


	vttype *oVt=malloc((dNet->maxId+1)*sizeof(vttype));
	assert(oVt!=NULL);
	vttype oNum;

	vttype *xVt=malloc((dNet->maxId+1)*sizeof(vttype));
	assert(xVt!=NULL);
	vttype xNum;

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

		memset(dNet->status, 0, (dNet->maxId+1)*sizeof(char));
		buildIStoDNet(IS, dNet);

		memcpy(oVt, IS->vt, IS->num*sizeof(vttype));
		oNum=IS->num;

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
					r=genrand_real1_threadsafe(isId);
					//r=0.5;
					if (r<touchRate) {
						if (dNet->status[neigh] == 0) {
							r=genrand_real1_threadsafe(isId);
							//r=0.5;
							if (r<infectRate) {
								dNet->status[neigh] = 1;
								xVt[xNum++]=neigh;
							}
						}
					}
				}
				dNet->status[vt] = 2;
			}

			vttype *temp = oVt; oVt = xVt; xVt = temp;
			oNum=xNum;
			++spreadStep;
		}

		vttype k, R=0;
		for (k=0; k<dNet->maxId+1; ++k) {
			if (dNet->status[k]==2) {
				++R;
			}
		}
		IR[l]=(double)R/(double)dNet->vtsNum;

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
	freeDNet(dNet);
	
	printf("IS Group %d:\tinfectRate:%f\tsp:%f\ts2p:%f\tfc:%f\tspreadStep:%f\n", IS->ISId, infectRate, sp, s2p, result, (double)spreadStep/(double)loopNum);fflush(stdout);
	return (void *)0;
}

int dnet_spread(struct InfectSourceFile * IS, struct DirectNet * dNet, double infectRate, double touchParam, int loopNum, int threadMax) {
	printf("begin to spread:\n");

	vttype isNum=IS->ISsNum;

	struct DNetSpreadCoreArgs args_thread[isNum];

	createThreadPool(threadMax);
	int i;
	for(i=0; i<isNum; ++i) {
		args_thread[i].dNet = dNet;
		args_thread[i].IS= IS->lines+i;
		args_thread[i].infectRate = infectRate;
		args_thread[i].touchParam = touchParam;
		args_thread[i].loopNum = loopNum;
		args_thread[i].isId= i;
		addWorktoThreadPool(dnet_spread_core, args_thread+i);
	}

	destroyThreadPool();

	//pthread_t threads[isNum];
	//struct DNetSpreadCoreArgs args_thread[isNum];

	//pthread_mutex_t mutex;
	//pthread_mutex_init(&mutex, NULL);
	//pthread_cond_t cond_thread;
	//pthread_cond_init(&cond_thread, NULL);
	//int threadNum = 0;
	//while (isNum-isId) {
	//	pthread_mutex_lock(&mutex);
	//	//printf("%d, isID:%d, isNum:%d\n", threadNum, isId, isNum);fflush(stdout);
	//	if (threadNum < threadMax) {
	//		args_thread[isId].dNet = dNet;
	//		args_thread[isId].IS= IS->lines+isId;
	//		args_thread[isId].infectRate = infectRate;
	//		args_thread[isId].touchParam = touchParam;
	//		args_thread[isId].loopNum = loopNum;
	//		args_thread[isId].mutex = &mutex;
	//		args_thread[isId].cond_thread = &cond_thread;
	//		args_thread[isId].isId= isId;
	//		pthread_create(threads+isId, NULL, dnet_spread_core, args_thread+isId);
	//		++threadNum;
	//		++isId;
	//	}
	//	else {
	//		pthread_cond_wait(&cond_thread, &mutex);
	//		--threadNum;
	//	}
	//	pthread_mutex_unlock(&mutex);
	//}
	//
	//vttype i;
	//for (i=0; i<isNum; ++i) {
	//	pthread_join(threads[i], NULL);
	//}
	//printf("OK! ALL!\n");
	//while(1);
	return 0;
}

struct DirectNet *cloneDNet(const struct DirectNet * const dnet) {
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

void *verifyDNet(void *arg) {
	struct DirectNet *dnet = arg;
	edtype i;
	vttype j;
	vttype *place = malloc((dnet->maxId+1)*sizeof(vttype));
	memset(place, -1, dnet->maxId+1);
	FILE *fp = fopen("data/duplicatePairsinDirectNet", "w");
	fileError(fp, "data/duplicatePairsinDirectNet");
	FILE *fp2 = fopen("data/NoDuplicatePairsNetFile", "w");
	fileError(fp2, "data/NoDuplicatePairsNetFile");
	fprintf(fp, "the following pairs are duplicate in the net file\n");
	char sign=0;
	for (j=0; j<dnet->maxId; ++j) {
		if (dnet->count[j]>0) {
			memset(place, -1, (dnet->maxId+1)*sizeof(vttype));
			for (i=0; i<dnet->count[j]; ++i) {
				vttype origin = dnet->to[j][i];
				vttype next = place[origin];
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
	return (void *)0;
}
