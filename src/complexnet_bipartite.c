#include "../inc/complexnet_bipartite.h"
#include "../inc/complexnet_error.h"
#include "../inc/complexnet_threadpool.h"
#include "../inc/complexnet_random.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

void free_Bipartite(struct Bipartite *Bip) {
	int i=0;
	for(i=0; i<Bip->maxId+1; ++i) {
		if (Bip->count[i]>0) {
			free(Bip->id[i]);
			free(Bip->i3[i]);
			free(Bip->i4[i]);
		}
	}
	free(Bip->count);
	free(Bip->id);
	free(Bip->i3);
	free(Bip->i4);
	free(Bip);
}

struct Bipartite *create_Bipartite(const struct i4LineFile * const file, int i1toi2) {
	//all elements of struct Bipartite.
	int maxId;
	int minId;
	int idNum;
	long countMax;
	long countMin;
	long *count;
	int **id;
	int **i3;
	int **i4;
	long edgesNum;

	//get maxId of i1 or i2.
	if (i1toi2) {
		maxId=file->i1Max;
		minId=file->i1Min;
	}
	else {
		maxId = file->i2Max;
		minId = file->i2Max;
	}

	//once get maxId, the four points can be assigned with memory.
	count=calloc(maxId+1, sizeof(long));
	assert(count!=NULL);
	id=malloc((maxId+1)*sizeof(void *));
	assert(id!=NULL);
	i3=malloc((maxId+1)*sizeof(void *));
	assert(i3!=NULL);
	i4=malloc((maxId+1)*sizeof(void *));
	assert(i4!=NULL);

	//fill the count.
	edgesNum=file->linesNum;
	struct i4Line *lines=file->lines;
	long i;
	if (i1toi2) {
		for(i=0; i<edgesNum; ++i) {
			++count[lines[i].i1];
		}
	}
	else {
		for(i=0; i<edgesNum; ++i) {
			++count[lines[i].i2];
		}
	}

	//once get count, the three points which point to point can be assigned with memory.
	int j;
	countMax = -1;
	countMin = LONG_MAX;
	for(j=0; j<maxId+1; ++j) {
		if (count[j]>0) {
			countMax = countMax>count[j]?countMax:count[j];
			countMin = countMin<count[j]?countMin:count[j];
			++idNum;
			id[i]=malloc(count[i]*sizeof(int));
			assert(id[i]!=NULL);
			i3[i]=malloc(count[i]*sizeof(int));
			assert(i3[i]!=NULL);
			i4[i]=malloc(count[i]*sizeof(int));
			assert(i4[i]!=NULL);
		}
		else {
			id[i] = NULL;
			i3[i] = NULL;
			i4[i] = NULL;
		}
	}

	//fill id, i3, i4
	long *temp = calloc(maxId+1, sizeof(long));
	assert(temp!=NULL);
	if (i1toi2) {
		for(i=0; i<edgesNum; ++i) {
			int i1 =lines[i].i1;
			id[i1][temp[i1]]=lines[i].i2;
			i3[i1][temp[i1]]=lines[i].i3;
			i4[i1][temp[i1]]=lines[i].i4;
			++temp[i1];
		}
	}
	else {
		for(i=0; i<edgesNum; ++i) {
			int i2 =lines[i].i2;
			id[i2][temp[i2]]=lines[i].i1;
			i3[i2][temp[i2]]=lines[i].i3;
			i4[i2][temp[i2]]=lines[i].i4;
			++temp[i2];
		}
	}
	free(temp);

	//fill Bip and return.
	struct Bipartite *Bip = malloc(sizeof(struct Bipartite));
	assert(Bip != NULL);

	Bip->maxId=maxId;
	Bip->minId=minId;
	Bip->idNum=idNum;
	Bip->countMax = countMax;
	Bip->countMin = countMin;
	Bip->count=count;
	Bip->id = id;
	Bip->i3 = i3;
	Bip->i4 = i4;
	Bip->edgesNum=edgesNum;

	if (i1toi2) {
		printf("build i1 net:\n\tMax: %d, Min: %d, Num: %d, countMax: %ld, countMin: %ld, edgesNum: %ld\n", maxId, minId, idNum, countMax, countMin, edgesNum); fflush(stdout);
	}
	else {
		printf("build i2 net:\n\tMax: %d, Min: %d, Num: %d, countMax: %ld, countMin: %ld, edgesNum: %ld\n", maxId, minId, idNum, countMax, countMin, edgesNum); fflush(stdout);
	}

	return Bip;
}

	/*
struct i4LineFile *abstractUser_Bipartite(struct Bipartite *bip) {

	int i1Max=bip->userMaxId;
	int i1Min=bip->userMinId;
	int i2Max=bip->itemMaxId;
	int i2Min=bip->itemMinId;
	int linesNum = bip->userNum;
	struct i4Line *lines = malloc(linesNum*sizeof(struct i4Line));
	assert(lines != NULL);

	for(i=0; i<i1Max+1; ++i) {
		if (bip->userCount[i]>0) {
			int random = genrand_int32(void) % userCount[i];
		}
		else {
			user[i] = NULL;
		}
	}
	for(i=0; i<itemMaxId+1; ++i) {
		if (itemCount[i]>0) {
			item[i]=malloc(itemCount[i]*sizeof(int));
			assert(item[i]!=NULL);
		}
		else {
			item[i] = NULL;
		}
	}

	long *temp_usercount=calloc(userMaxId+1, sizeof(long));
	assert(temp_usercount!=NULL);
	long *temp_itemcount=calloc(itemMaxId+1, sizeof(long));
	assert(temp_itemcount!=NULL);

	for(i=0; i<linesNum; ++i) {
		int i1 =lines[i].i1;
		int i2 =lines[i].i2;
		user[i1][temp_usercount[i1]++]=i2;
		item[i2][temp_itemcount[i2]++]=i1;
	}
	free(temp_usercount);
	free(temp_itemcount);

	struct Bipartite *Bip = malloc(sizeof(struct Bipartite));
	assert(Bip != NULL);

	Bip->userMaxId=userMaxId;
	Bip->userMinId=userMinId;
	Bip->userNum=userNum;
	Bip->itemMaxId=itemMaxId;
	Bip->itemMinId=itemMinId;
	Bip->itemNum=itemNum;

	Bip->edgesNum=linesNum;
	Bip->userCount=userCount;
	Bip->itemCount=itemCount;
	Bip->user = user;
	Bip->item = item;
	printf("build net:\n\tuserMax: %d, userMin: %d, userNum: %d\n\titemMax: %d, itemMin: %d, itemNum: %d\n\tedgesNum: %ld\n", userMaxId, userMinId, userNum, itemMaxId, itemMinId, itemNum, linesNum); fflush(stdout);
	int i;
	for (i=0; i<bip->userMaxId+1; ++i) {
		if (userCount[i] > 0) {

		}
	}
}
	*/
