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
	int *inDoorMax = calloc(maxId+1, sizeof(int));
	assert(inDoorMax != NULL);
	int *inDoorMin = malloc((maxId+1)*sizeof(int));
	assert(inDoorMin != NULL);
	long i;
	for (i=0; i<maxId+1; ++i) {
		inDoorMin[i] = INT_MAX;
	}
	long *outCount = calloc(maxId+1, sizeof(long));
	assert(outCount != NULL);
	long *inCount = calloc(maxId+1, sizeof(long));
	assert(inCount != NULL);
	int **out = calloc(maxId+1, sizeof(void *));
	assert(out != NULL);
	int **outTemporal = calloc(maxId+1, sizeof(void *));	
	assert(out != NULL);

	struct i4Line *line;
	for (i=0; i<file->linesNum; ++i) {
		line=file->lines+i;
		++outCount[line->i1];
		++inCount[line->i2];
		inDoorMax[line->i2] = inDoorMax[line->i2]<line->i4?line->i4:inDoorMax[line->i2];
		inDoorMin[line->i2] = inDoorMin[line->i2]>line->i4?line->i4:inDoorMin[line->i2];
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
		}
	}
	long *outCount_temp = calloc(maxId+1, sizeof(long));
	assert(outCount_temp != NULL);
	for (i=0; i<file->linesNum; ++i) {
		int outId = file->lines[i].i1;
		out[outId][outCount_temp[outId]]=file->lines[i].i2;
		outTemporal[outId][outCount_temp[outId]]=file->lines[i].i4;
		++outCount_temp[outId];
	}
	free(outCount_temp);

	dtnet.maxId=maxId;
	dtnet.minId=minId;
	dtnet.vtsNum=vtsNum;
	dtnet.edgesNum=edgesNum;
	dtnet.inCountMax=inCountMax;
	dtnet.outCountMax=outCountMax;
	dtnet.inDoorMax=inDoorMax;
	dtnet.inDoorMin=inDoorMin;
	dtnet.inCount=inCount;
	dtnet.outCount=outCount;
	dtnet.out=out;
	dtnet.outTemporal=outTemporal;
	dtnet.timeMax=timeMax;
	dtnet.timeMin=timeMin;

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
		}
	}
	free(dtnet.inDoorMax);
	free(dtnet.inDoorMin);
	free(dtnet.outCount);
	free(dtnet.inCount);
	free(dtnet.out);
	free(dtnet.outTemporal);
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

int shortpath_11_DTNet(int id_from, int id_to) {
	int door = dtnet.inDoorMax[id_to];
	int ground = INT_MAX;
	int *lt = malloc((dtnet.maxId+1)*sizeof(int));
	int *rt = malloc((dtnet.maxId+1)*sizeof(int));
	char *status = calloc((dtnet.maxId+1), sizeof(char));
	char *status_l = calloc((dtnet.maxId+1), sizeof(char));
	char *status_r = calloc((dtnet.maxId+1), sizeof(char));
	int l_num=0, r_num=0;
	int i;
	for (i=0; i<dtnet.outCount[id_from]; ++i) {
		int t = dtnet.outTemporal[id_from][i];
		int v = dtnet.out[id_from][i];
		if (t> door) {
			continue;
		}
		ground = ground>t?t:ground;
		if (status_l[v] == 1) {
			lt[v]= lt[v]>t?t:lt[v];
		}
		else {
			lt[v]=t;
			status_l[v]=1;
			++l_num;
		}
	}
	status[id_from]=1;

	for(i=0; i<dtnet.maxId+1; ++i) {
		if (status_l[i] == 1) {
			printf("%d,%d|||", i, lt[i]);
		}
	}
	printf("\n");fflush(stdout);
	printf("door: %d\n", door);

	int j=0;
	for (i=0; i<dtnet.maxId+1; ++i) {
		if (dtnet.inCount[i] == 0 && dtnet.outCount[i] !=0) {
			++j;
		}
	}
	printf("j:%d\n", j);
	exit(-1);
	char sign = 0;
	int ground2=INT_MAX;
	while(l_num) {
		ground2=INT_MAX;
		printf("l_num: %d,%d,%d,%d,%d\n", l_num, door, ground, dtnet.inDoorMin[3], dtnet.inDoorMax[3]);
		/*
		for(i=0; i<dtnet.maxId+1; ++i) {
			if (status_l[i] == 1) {
				printf("%d,%d|||", i, lt[i]);
			}
		}
		printf("\n");
		*/
		for(i=0; i<dtnet.maxId+1; ++i) {
			if (status_l[i] == 0) {
				continue;
			}
			int point_t = lt[i];
			if (i == id_to) {
				if (point_t==dtnet.inDoorMin[id_to]) {
					return point_t;
				}
				else {
					door = point_t;
					sign = 1;
				}
			}
			for (j=0; j<dtnet.outCount[i];++j) {
				int neigh = dtnet.out[i][j];
				int neigh_t = dtnet.outTemporal[i][j];
				if (neigh_t>=point_t && neigh_t<=door && status[neigh] == 0) {
					ground2 = ground2>neigh_t?neigh_t:ground2;
					if (status_r[neigh] == 1) {
						rt[neigh] = rt[neigh]>neigh_t?neigh_t:rt[neigh];
					}
					else {
						rt[neigh]=neigh_t;
						++r_num;
						status_r[neigh] = 1;
					}
				}
			}
			//if (dtnet.outDoorMax[i]<ground) {
			//	status[i]=1;
			//}
		}
		ground = ground2;
		l_num=r_num;
		char *temp;
		temp=status_l; status_l=status_r; status_r=temp;
		int *temp_t;
		temp_t=lt; lt=rt; rt=temp_t;
		memset(status_r, 0, dtnet.maxId+1);
		r_num=0;
	}
	if (sign ==1) {
		return door;
	}
	return -1;
}
