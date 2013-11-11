#include "../inc/complexnet_bipartite.h"
#include "../inc/utility/error.h"
#include "../inc/utility/random.h"
#include "../inc/utility/sort.h"
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

	int origin = bip->idNum;
	renew_Bipartite(bip);

	struct i4LineFile *file = malloc(sizeof(struct i4LineFile));
	assert(file != NULL);
	file->i1Max = i1Max;
	file->i1Min = i1Min;
	file->linesNum = j;
	file->lines = lines;
	printf("abstract_Bipartite done:\n\tBipartite originally has %d ids.\n\tabstract %d edges from %d.\n", origin, j, linesNum);
	printf("\tthere are %d ids whose degree has become 0 after abstract.\n", k);
	printf("\tNow Bipartite has %d ids.\n", bip->idNum);
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
	printf("backtofile_Bipartite done. %d lines generated.\n", k);
	return file;
}

// if count is 20, then the id whose count is 0-19 will be deleted.
void cutcount_Bipartite(struct Bipartite *bip, long count) {
	if (count > bip->countMax || count < 0) {
		printf("cutcount_Bipartite error: wrong count.\n");
		return;
	}
	int i, j=0;
	for (i=0; i<bip->maxId + 1; ++i) {
		if (bip->count[i] > 0 && bip->count[i] < count) {
			bip->count[i] = 0;
			free(bip->id[i]);
			free(bip->i3[i]);
			free(bip->i4[i]);
			++j;
		}
	}
	int origin = bip->idNum;
	renew_Bipartite(bip);
	printf("cutcount_Bipartite done:\n\tBipartite originally has %d ids.\n\tthere are %d ids whose count < %ld being deleted.\n\tNow Bipartite has %d ids.\n", origin, j, count, bip->idNum);fflush(stdout);
}

void sortBytime_Bipartite(struct Bipartite *bip) {
	int i;
	for (i=0; i < bip->maxId + 1; ++i) {
		if (bip->count[i] > 0) {
			quick_sort_int_index_index(bip->i4[i], 0, bip->count[i]-1, bip->id[i], bip->i3[i]);
		}
	}
}

struct i4LineFile *divideBytime_Bipartite(struct Bipartite *bip, double rate) {
	if (rate <=0 || rate >= 1) {
		printf("divideBytime_Bipartite error: wrong rate.\n");
		return NULL;
	}

	int l1, l2;
	if (bip->edgesNum > 100000) {
		l1 = (int)(bip->edgesNum*(rate+0.1));
		l2 = (int)(bip->edgesNum*(1-rate+0.1));
	}
	else {
		l2 = l1 = bip->edgesNum;
	}

	struct i4LineFile *twofile = malloc(2*sizeof(struct i4LineFile));
	assert(twofile != NULL);

	twofile[0].lines = malloc(l1*sizeof(struct i4Line));
	assert(twofile[0].lines != NULL);
	twofile[1].lines = malloc(l2*sizeof(struct i4Line));
	assert(twofile[1].lines != NULL);

	int line1=0, line2=0;
	int i1Max=-1; 
	int i2Max=-1;
    int i1Min=INT_MAX;
    int i2Min=INT_MAX;
	int _i1Max=-1; 
	int _i2Max=-1;
    int _i1Min=INT_MAX;
    int _i2Min=INT_MAX;

	int i;
	long j;
	for (i=0; i<bip->maxId + 1; ++i) {
		if (bip->count[i] > 0) {
			int div = (int)(bip->count[i] * rate);
			if (div <=0 || div == bip->count[i]) {
				printf("divideBytime_Bipartite error: id %d has too small count %ld.\n", i, bip->count[i]);
				return NULL;
			}
			for (j = 0; j < div; ++j) {
				twofile[0].lines[line1].i1 = i;
				twofile[0].lines[line1].i2 = bip->id[i][j];
				twofile[0].lines[line1].i3 = bip->i3[i][j];
				twofile[0].lines[line1].i4 = bip->i4[i][j];
				i1Max = i;
				i1Min = i1Min<i?i1Min:i;
				i2Max = i2Max>bip->id[i][j]?i2Max:bip->id[i][j];
				i2Min = i2Min<bip->id[i][j]?i2Min:bip->id[i][j];
				++line1;
			}
			for (j=div; j<bip->count[i]; ++j) {
				twofile[1].lines[line2].i1 = i;
				twofile[1].lines[line2].i2 = bip->id[i][j];
				twofile[1].lines[line2].i3 = bip->i3[i][j];
				twofile[1].lines[line2].i4 = bip->i4[i][j];
				_i1Max = i;
				_i1Min = _i1Min<i?_i1Min:i;
				_i2Max = _i2Max>bip->id[i][j]?_i2Max:bip->id[i][j];
				_i2Min = _i2Min<bip->id[i][j]?_i2Min:bip->id[i][j];
				++line2;
			}
		}
	}

	if (line1>l1 || line2 >l2) {
		printf("divideBytime_Bipartite error: l1/l2 too small\n");
		return NULL;
	}
	twofile[0].linesNum = line1;
	twofile[0].i1Max = i1Max;
	twofile[0].i2Max = i2Max;
	twofile[0].i1Min = i1Min;
	twofile[0].i2Min = i2Min;

	twofile[1].linesNum = line2;
	twofile[1].i1Max = _i1Max;
	twofile[1].i2Max = _i2Max;
	twofile[1].i1Min = _i1Min;
	twofile[1].i2Min = _i2Min;
	printf("divideBytime_Bipartite done:\n\trate: %f\n\tfile1: linesNum: %d, i1Max: %d, i1Min: %d, i2Max: %d, i2Min: %d\n\tfile2: linesNum: %d, i1Max: %d, i1Min: %d, i2Max: %d, i2Min: %d\n", rate, line1, i1Max, i1Min, i2Max, i2Min, line2, _i1Max, _i1Min, _i2Max, _i2Min);fflush(stdout);
	return twofile;
}
