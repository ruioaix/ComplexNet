#include "../inc/complexnet_dtnet.h"
#include "../inc/complexnet_error.h"
#include "../inc/complexnet_bitsign.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

static struct DirectTimeNet dtnet;

void init_DirectTimeNet(const struct i4LineFile* const file) {
	int maxId=file->iMax;
	int minId=file->iMin;
	int vtsNum=0;
	long edgesNum=file->linesNum;
	long inCountMax=-1;
	long outCountMax=-1;
	int timeMax=-1;
	int timeMin=INT_MAX;
	int *inTimeDoor = calloc(maxId+1, sizeof(int));
	assert(inTimeDoor != NULL);
	long *outCount = calloc(maxId+1, sizeof(long));
	assert(outCount != NULL);
	long *inCount = calloc(maxId+1, sizeof(long));
	assert(inCount != NULL);
	int **out = calloc(maxId+1, sizeof(void *));
	assert(out != NULL);
	int **outTime = calloc(maxId+1, sizeof(void *));	
	assert(out != NULL);

	long i;
	struct i4Line *line;
	for (i=0; i<file->linesNum; ++i) {
		line=file->lines+i;
		++outCount[line->i1];
		++inCount[line->i2];
		if (inTimeDoor[line->i2] < line->i4) {
			inTimeDoor[line->i2] = line->i4;
		}
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
			outTime[j] = malloc(outCount[j]*sizeof(int));
			assert(outTime[j]!=NULL);
		}
	}
	long *outCount_temp = calloc(maxId+1, sizeof(long));
	assert(outCount_temp != NULL);
	for (i=0; i<file->linesNum; ++i) {
		int outId = file->lines[i].i1;
		out[outId][outCount_temp[outId]]=file->lines[i].i2;
		outTime[outId][outCount_temp[outId]]=file->lines[i].i4;
		++outCount_temp[outId];
	}
	free(outCount_temp);

	dtnet.maxId=maxId;
	dtnet.minId=minId;
	dtnet.vtsNum=vtsNum;
	dtnet.edgesNum=edgesNum;
	dtnet.inCountMax=inCountMax;
	dtnet.outCountMax=outCountMax;
	dtnet.inTimeDoor=inTimeDoor;
	dtnet.inCount=inCount;
	dtnet.outCount=outCount;
	dtnet.out=out;
	dtnet.outTime=outTime;
	dtnet.timeMax=timeMax;
	dtnet.timeMin=timeMin;

	printf("build direct time net:\n\tMax: %d, Min: %d, vtsNum: %d\n", maxId, minId, vtsNum); 
	printf("\tedgesNum: %ld, inCountMax: %ld, outCountMax: %ld\n", edgesNum, inCountMax, outCountMax);
	printf("\ttimeMax: %d, timeMin:%d\n", timeMax, timeMin); 
	fflush(stdout);
}

void *verifyDTNet(void *arg) {

	long i;
	int j;

	int *place = malloc((dtnet.maxId+1)*sizeof(int));
	memset(place, -1, dtnet.maxId+1);
	FILE *fp = fopen("data/duplicatePairsinDirectNet", "w");
	fileError(fp, "data/duplicatePairsinDirectNet");
	FILE *fp2 = fopen("data/NoDuplicatePairsNetFile", "w");
	fileError(fp2, "data/NoDuplicatePairsNetFile");
	fprintf(fp, "the following pairs are duplicate in the net file\n");
	char sign=0;

	long temp=0;
	for (j=0; j<dtnet.maxId; ++j) {
		temp+=dtnet.outCount[j];
	}
	printf("%ld, %ld\n", temp, dtnet.edgesNum);
	for (j=0; j<dtnet.maxId; ++j) {
		if (dtnet.outCount[j]>0) {
			memset(place, -1, (dtnet.maxId+1)*sizeof(int));
			for (i=0; i<dtnet.outCount[j]; ++i) {
				int origin = dtnet.out[j][i];
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
