#include "../inc/complexnet_dtnet.h"
#include "../inc/complexnet_error.h"
#include "../inc/complexnet_bitsign.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

static struct DirectTemporalNet dtnet;

void init_DirectTemporalNet(const struct i4LineFile* const file) {
	int maxId=file->iMax;
	int minId=file->iMin;
	int vtsNum=0;
	long edgesNum=file->linesNum;
	long inCountMax=-1;
	long outCountMax=-1;
	int timeMax=-1;
	int timeMin=INT_MAX;
	long i;
	long *outCount = calloc(maxId+1, sizeof(long));
	assert(outCount != NULL);
	long *inCount = calloc(maxId+1, sizeof(long));
	assert(inCount != NULL);
	int **out = calloc(maxId+1, sizeof(void *));
	assert(out != NULL);
	int **outTemporal = calloc(maxId+1, sizeof(void *));	
	assert(out != NULL);
	int **outTemporal_second01 = calloc(maxId+1, sizeof(void *));	
	assert(out != NULL);

	struct i4Line *line;
	for (i=0; i<file->linesNum; ++i) {
		line=file->lines+i;
		++outCount[line->i1];
		++inCount[line->i2];
		timeMax=timeMin<line->i4?line->i4:timeMax;
		timeMin=timeMin>line->i4?line->i4:timeMin;
	}
	int j;
	for (j=0; j<maxId+1; ++j) {
		if (outCount[j] !=0 || inCount[j] !=0) {
			++vtsNum;
		}
		if (outCount[j]>outCountMax) {
			outCountMax = outCount[j];
		}
		if (inCount[j]>inCountMax) {
			inCountMax = inCount[j];
		}
		if (outCount[j]>0) {
			out[j] = malloc(outCount[j]*sizeof(int));
			assert(out[j]!=NULL);
			outTemporal[j] = malloc(outCount[j]*sizeof(int));
			assert(outTemporal[j]!=NULL);
			outTemporal_second01[j] = malloc(outCount[j]*sizeof(int));
			assert(outTemporal[j]!=NULL);
		}
	}
	long *outCount_temp = calloc(maxId+1, sizeof(long));
	assert(outCount_temp != NULL);
	for (i=0; i<file->linesNum; ++i) {
		int outId = file->lines[i].i1;
		out[outId][outCount_temp[outId]]=file->lines[i].i2;
		outTemporal[outId][outCount_temp[outId]]=file->lines[i].i4;
		outTemporal_second01[outId][outCount_temp[outId]]=file->lines[i].i4;
		++outCount_temp[outId];
	}
	free(outCount_temp);

	dtnet.maxId=maxId;
	dtnet.minId=minId;
	dtnet.vtsNum=vtsNum;
	dtnet.edgesNum=edgesNum;
	dtnet.inCountMax=inCountMax;
	dtnet.outCountMax=outCountMax;
	dtnet.inCount=inCount;
	dtnet.outCount=outCount;
	dtnet.out=out;
	dtnet.outTemporal=outTemporal;
	dtnet.outTemporal_second01=outTemporal_second01;
	dtnet.timeMax=timeMax;
	dtnet.timeMin=timeMin;
	dtnet.timeScope=second01;

	printf("build direct time net:\n\tMax: %d, Min: %d, vtsNum: %d\n", maxId, minId, vtsNum); 
	printf("\tedgesNum: %ld, inCountMax: %ld, outCountMax: %ld\n", edgesNum, inCountMax, outCountMax);
	printf("\ttimeMax: %d, timeMin:%d\n", timeMax, timeMin); 
	fflush(stdout);
}
void free_DirectTemporalNet(void) {
	
	int i;
	for (i=0; i<dtnet.maxId+1; ++i) {
		if (dtnet.outCount[i]>0) {
			free(dtnet.out[i]);
			free(dtnet.outTemporal[i]);
			free(dtnet.outTemporal_second01[i]);
		}
	}
	free(dtnet.outCount);
	free(dtnet.inCount);
	free(dtnet.out);
	free(dtnet.outTemporal);
	free(dtnet.outTemporal_second01);
}

void setTimeScope_DirectTemporalNet(enum timeScopeDTNet timeScope) {
	if (dtnet.timeScope == timeScope) {
		printf("timeScope of DirectTemporalNet has been set to %d.\n", timeScope);fflush(stdout);
		return;
	}
	int i;
	long j;
	for (i=0; i<dtnet.maxId+1; ++i) {
		for (j=0; j<dtnet.outCount[i]; ++j) {
			dtnet.outTemporal[i][j] = dtnet.outTemporal_second01[i][j]%timeScope;	
		}
	}
	dtnet.timeScope = timeScope;
	printf("timeScope of DirectTemporalNet has been set to %d.\n", timeScope);fflush(stdout);
}

void *verifyDTNet(void *arg) {

	long i;
	int j;

	int *place = malloc((dtnet.maxId+1)*sizeof(int));
	int *l1= malloc((dtnet.outCountMax+1)*sizeof(int));
	int *l2= malloc((dtnet.outCountMax+1)*sizeof(int));
	int *r1= malloc((dtnet.outCountMax+1)*sizeof(int));
	int *r2= malloc((dtnet.outCountMax+1)*sizeof(int));
	memset(place, -1, dtnet.maxId+1);
	FILE *fp = fopen("data/duplicatePairsinDirectTemporalNet", "w");
	fileError(fp, "data/duplicatePairsinDirectTemporalNet");
	FILE *fp2 = fopen("data/NoDuplicatePairsDirectTemporalFile", "w");
	fileError(fp2, "data/NoDuplicatePairsDirectTemporalFile");
	fprintf(fp, "the following pairs are duplicate in the net file\n");
	char sign=0;

	for (j=0; j<dtnet.maxId; ++j) {
		if (dtnet.outCount[j]>0) {
			int l_i=dtnet.outCount[j];
			memset(place, -1, (dtnet.maxId+1)*sizeof(int));
			memcpy(l1, dtnet.out[j], l_i*sizeof(int));
			memcpy(l2, dtnet.outTemporal[j], l_i*sizeof(int));
			int r_i=0;
			do {
				for (i=0; i<l_i; ++i) {
					int id=l1[i];	
					if (place[id] == -1) {
						place[id] = l2[i];
						fprintf(fp2, "%d\t%d\t1\t%d\n", j, id, l2[i]);
					}
					else if (place[id] == l2[i]) {
						fprintf(fp, "%d\t%d\t1\t%d\n", j, id, l2[i]);
						sign=1;
					}
					else {
						r1[r_i]=l1[i];
						r2[r_i]=l2[i];
						++r_i;
					}
				}
				memcpy(l1, r1, r_i*sizeof(int));
				memcpy(l2, r2, r_i*sizeof(int));
				memset(place, -1, (dtnet.maxId+1)*sizeof(int));
				l_i=r_i;
				r_i=0;
			} while(l_i);
		}
	}
	free(place);
	free(l1);
	free(l2);
	free(r1);
	free(r2);
	fclose(fp);
	fclose(fp2);
	if (sign == 1) {
		isError("the file has duplicate pairs, you can check data/duplicatePairsinDirectTemporalNet.\n\twe generate a net file named data/NoDuplicatePairsDirectTemporalFile which doesn't contain any duplicate pairsr.\n\tyou should use this file instead the origin wrong one.\n");
	}
	else {
		printf("verifyDNet: perfect network.\n");
	}
	return (void *)0;
}

void *shortpath_1n_DTNet(void *arg) {

	struct DTNetShortPath1NArgs *args=arg;

	int id_from = args->vtId;
	FILE *fp = args->fp;

	if (dtnet.outCount[id_from] == 0) {
		printf("%d have no out edges.\n", id_from);
		return (void *)1;
	}
	char *status = calloc(dtnet.maxId+1, sizeof(char));
	assert(status!=NULL);
	int *sp = malloc((dtnet.maxId+1)*sizeof(int));
	assert(sp!=NULL);

	int tMin=INT_MAX;
	int vMin;
	int i;
	for (i=0; i<dtnet.outCount[id_from]; ++i) {
		int v=dtnet.out[id_from][i];
		int t=dtnet.outTemporal[id_from][i];
		if (tMin>t) {
			tMin=t;
			vMin=v;
		}
		if (status[v] == 0) {
			sp[v]=t;
			status[v] = 1;
		}
		else if (status[v] == 1) {
			sp[v]=sp[v]>t?t:sp[v];
		}
	}
	int j=0;
	while (tMin != INT_MAX) {
		for (i=0; i<dtnet.outCount[vMin]; ++i) {
			int v=dtnet.out[vMin][i];
			int t=dtnet.outTemporal[vMin][i];
			if (t>=tMin) {
				if (status[v] == 0) {
					sp[v]=t;
					status[v] = 1;
				}
				else if (status[v] == 1) {
					sp[v]=sp[v]>t?t:sp[v];
				}
			}
		}
		status[vMin]=2;
		tMin = INT_MAX;
		for (i=0; i<dtnet.maxId+1; ++i) {
			if (status[i] == 1) {
				if(tMin>sp[i]) {
					tMin=sp[i];
					vMin=i;
				}
			}
		}
		++j;
	}

	for (i=0; i<dtnet.maxId+1; ++i) {
		if (status[i] == 2) {
			fprintf(fp, "%d\t%d\t%d\n", id_from, i, sp[i]);
		}
	}
	printf("%d done\n", id_from);fflush(stdout);
	
	free(status);
	free(sp);
	return (void *)0;
}

int getMaxId_DirectTemporalNet() {
	return dtnet.maxId;
}

int gettimeScope_DirectTemporalNet() {
	return dtnet.timeScope;
}
