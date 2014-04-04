#include "iinetd.h"
#include "error.h"
#include "mt_random.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

void free_iiNetD(struct iiNetD *net) {
	free(net.count);
	int i=0;
	for(i=0; i<net.maxId+1; ++i) {
		free(net.to[i]);
	}
	free(net.to);
	free(net);
}

void create_iiNetD(const struct iiLineFile * const file) {
	int maxId=file->i1Max>file->i2Max?file->i1Max:file->i2Max;
	int minId=file->i1Min<file->i2Min?file->i1Min:file->i2Min;
	long linesNum=file->linesNum;
	struct iiLine *lines=file->lines;

	long *count=calloc(maxId+1, sizeof(long));
	assert(count!=NULL);

	long *temp_count=calloc(maxId+1, sizeof(long));
	assert(temp_count!=NULL);

	long i;
	for(i=0; i<linesNum; ++i) {
		++count[lines[i].i1];
		++temp_count[lines[i].i2];
		++temp_count[lines[i].i1];
	}
	int j;
	int idNum=0;
	for(j=0; j<maxId+1; ++j) {
		if (temp_count[j]>0) {
			++idNum;
		}
	}

	int **to=malloc((maxId+1)*sizeof(void *));
	assert(to!=NULL);
	long countMax=count[0];
	long countMin = count[0];
	for(i=0; i<maxId+1; ++i) {
		countMax = countMax > count[i] ? countMax : count[i];
		countMin = countMin < count[i] ? countMin : count[i];
		if (count[i]!=0) {
			to[i]=malloc(count[i]*sizeof(int));
			assert(to[i]!=NULL);
		}
		else {
			to[i] = NULL;
		}
	}

	memset(temp_count, 0, (maxId+1)*sizeof(long));
	for(i=0; i<linesNum; ++i) {
		int id_from=lines[i].i1;
		int id_to=lines[i].i2;
		to[id_from][temp_count[id_from]++]=id_to;
	}
	free(temp_count);

	struct iiNetD *net=malloc(sizeof(struct DirectNet));
	assert(net != NULL);
	net.maxId=maxId;
	net.minId=minId;
	net.linesNum=linesNum;
	net.idNum=idNum;
	net.countMax=countMax;
	net.countMin=countMin;
	net.count=count;
	net.to=to;
	printf("create direct net:\n\tMax: %d, Min: %d, idNum: %d, linesNum: %ld, countMax: %ld, countMin: %ld\n", maxId, minId, idNum, linesNum, countMax, countMin); fflush(stdout);
}

void *verify_iiNetD(struct iiNetD *net) {
	long i;
	int j;
	int *place = malloc((net.maxId+1)*sizeof(int));
	memset(place, -1, net.maxId+1);
	FILE *fp = fopen("data/duplicatePairsinDirectNet", "w");
	fileError(fp, "data/duplicatePairsinDirectNet");
	FILE *fp2 = fopen("data/NoDuplicatePairsNetFile", "w");
	fileError(fp2, "data/NoDuplicatePairsNetFile");
	fprintf(fp, "the following pairs are duplicate in the net file\n");
	char sign=0;
	for (j=0; j<net.maxId; ++j) {
		if (net.count[j]) {
			memset(place, 0, (net.maxId+1)*sizeof(int));
			for (i=0; i<net.count[j]; ++i) {
				int origin = net.to[j][i];
				int next = place[origin];
				if (next) {
					fprintf(fp, "%d\t%d\n", j, next);
					sign=1;
				}
				else {
					place[origin]=1;
					fprintf(fp2, "%d\t%d\n", j,origin);
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
		printf("verify_iiNetD: perfect network.\n");
	}
	return (void *)0;
}
