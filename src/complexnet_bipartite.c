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

//struct Bipartite {
//	int maxId;
//	int minId;
//	int idNum;
//	long countMax;
//	long countMin;
//	long *count;
//	int **id;
//	int **i3;
//	int **i4;
//	long edgesNum;
//};

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
	int idNum=0;
	long countMax=-1;
	long countMin=LONG_MAX;
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
		minId = file->i2Min;
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
	for(j=0; j<maxId+1; ++j) {
		if (count[j]>0) {
			countMax = countMax>count[j]?countMax:count[j];
			countMin = countMin<count[j]?countMin:count[j];
			++idNum;
			id[j]=malloc(count[j]*sizeof(int));
			assert(id[j]!=NULL);
			i3[j]=malloc(count[j]*sizeof(int));
			assert(i3[j]!=NULL);
			i4[j]=malloc(count[j]*sizeof(int));
			assert(i4[j]!=NULL);
		}
		else {
			id[j] = NULL;
			i3[j] = NULL;
			i4[j] = NULL;
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

static void renew_Bipartite(struct Bipartite *bip) {
	int i;
	long countMax = -1;
	long countMin = LONG_MAX;
	for (i=bip->minId; i< bip->maxId + 1; ++i) {
		if (bip->count[i] > 0) {
			bip->minId = i;
			break;
		}
	}
	for (i=bip->maxId; i > bip->maxId - 1; --i) {
		if (bip->count[i] > 0) {
			bip->maxId = i;
			break;
		}
	}
	int idNum = 0;
	long edgesNum = 0;
	for (i=0; i< bip->maxId + 1; ++i) {
		if (bip->count[i] > 0) {
			edgesNum += bip->count[i];
			++idNum;
			countMax = countMax>bip->count[i]?countMax:bip->count[i];
			countMin = countMin<bip->count[i]?countMin:bip->count[i];
		}
	}
	bip->countMax = countMax;
	bip->countMin = countMin;
	bip->idNum = idNum;
	bip->edgesNum = edgesNum;
}

//have to remeber: after being abstract, the bip changed.
//and bip can be used as normal.
//but its idNum/maxId/minId/edgesNum/countMax/countMin may be different from the origin one.
//e.g. you get 5000 lines from the first calling of abstract_Bipartite, but maybe you can only get
//4990 lines from the second calling. that means in the origin bip, there are 10 id whose count is 1.
//get it?
struct i4LineFile *abstract_Bipartite(struct Bipartite *bip) {

	int i1Max=bip->maxId;
	int i1Min=bip->minId;
	
	int linesNum = bip->idNum;
	struct i4Line *lines = malloc(linesNum*sizeof(struct i4Line));
	assert(lines != NULL);

	int i;
	int j=0;
	int k=0;
	for(i=0; i<i1Max+1; ++i) {
		int degree = bip->count[i];
		if (degree > 0) {
			int random = genrand_int31() % degree;
			lines[j].i1 = i;
			lines[j].i2 = bip->id[i][random];
			lines[j].i3 = bip->i3[i][random];
			lines[j].i4 = bip->i4[i][random];
			++j;
			if (random == (degree - 1)) {
				--bip->count[i];
				if (bip->count[i] == 0) {
					//printf("%d, ", i);
					free(bip->id[i]);
					free(bip->i3[i]);
					free(bip->i4[i]);
					++k;
				}
			}
			else {
				bip->id[i][random] = bip->id[i][degree-1];
				bip->i3[i][random] = bip->i3[i][degree-1];
				bip->i4[i][random] = bip->i4[i][degree-1];
				--bip->count[i];
			}
		}
	}

	renew_Bipartite(bip);

	struct i4LineFile *file = malloc(sizeof(struct i4LineFile));
	assert(file != NULL);
	file->i1Max = i1Max;
	file->i1Min = i1Min;
	file->linesNum = j;
	file->lines = lines;
	printf("abstract_Bipartite done:\n\tabstract %d edges from %d.\n", j, linesNum);
	printf("\tthere are %d ids whose degree has become 0 after abstract.\n", k);
	return file;
}

//remeber: only i1Max and i2Min are useful, i2Max and i2Min is not useful.
//I can make i2Max and i2Min meanful, but it's time-consuming and useless in this situation. 
//so I just set them to -1.
struct i4LineFile *backtofile_Bipartite(struct Bipartite *bip) {
	int i1Max=bip->maxId;
	int i1Min=bip->minId;

	int linesNum = bip->edgesNum;
	struct i4Line *lines = malloc(linesNum*sizeof(struct i4Line));
	assert(lines != NULL);

	int i;
	int j=0;
	int k=0;
	for(i=0; i<i1Max+1; ++i) {
		if (bip->count[i] > 0) {
			for (j=0; j< bip->count[i]; ++j) {
				lines[k].i1 = i;
				lines[k].i2 = bip->id[i][j];
				lines[k].i3 = bip->i3[i][j];
				lines[k].i4 = bip->i4[i][j];
				++k;
			}
		}
	}

	struct i4LineFile *file = malloc(sizeof(struct i4LineFile));
	assert(file != NULL);
	file->i1Max = i1Max;
	file->i1Min = i1Min;
	file->i2Max = -1;
	file->i2Min = -1;
	file->linesNum = k;
	file->lines = lines;
	printf("backtofile_Bipartite done:\n\t%d lines generated.\n", k);
	return file;
}
