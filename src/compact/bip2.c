#include "../../inc/compact/bip2.h"
#include "../../inc/utility/error.h"
#include "../../inc/utility/random.h"
#include "../../inc/utility/sort.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

//struct Bip2 {
//	int maxId;
//	int minId;
//	int idNum;
//	long countMax;
//	long countMin;
//	long *count;
//	int **id;
//	long edgesNum;
//};

void free_Bip2(struct Bip2 *Bip) {
	int i=0;
	for(i=0; i<Bip->maxId+1; ++i) {
		if (Bip->count[i]>0) {
			free(Bip->id[i]);
		}
	}
	free(Bip->count);
	free(Bip->id);
	free(Bip);
}

struct Bip2 *create_Bip2(const struct iiLineFile * const file, int i1toi2) {
	//all elements of struct Bip2.
	int maxId;
	int minId;
	int idNum=0;
	long countMax=-1;
	long countMin=LONG_MAX;
	long *count;
	int **id;
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

	//fill the count.
	edgesNum=file->linesNum;
	struct iiLine *lines=file->lines;
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
		}
		else {
			id[j] = NULL;
		}
	}


	//fill id, i3, i4
	long *temp = calloc(maxId+1, sizeof(long));
	assert(temp!=NULL);
	if (i1toi2) {
		for(i=0; i<edgesNum; ++i) {
			int i1 =lines[i].i1;
			id[i1][temp[i1]]=lines[i].i2;
			++temp[i1];
		}
	}
	else {
		for(i=0; i<edgesNum; ++i) {
			int i2 =lines[i].i2;
			id[i2][temp[i2]]=lines[i].i1;
			++temp[i2];
		}
	}
	free(temp);

	//fill Bip and return.
	struct Bip2 *Bip = malloc(sizeof(struct Bip2));
	assert(Bip != NULL);

	Bip->maxId=maxId;
	Bip->minId=minId;
	Bip->idNum=idNum;
	Bip->countMax = countMax;
	Bip->countMin = countMin;
	Bip->count=count;
	Bip->id = id;
	Bip->edgesNum=edgesNum;

	if (i1toi2) {
		printf("build i1->i2 net:\n\tMax: %d, Min: %d, Num: %d, countMax: %ld, countMin: %ld, edgesNum: %ld\n", maxId, minId, idNum, countMax, countMin, edgesNum); fflush(stdout);
	}
	else {
		printf("build i2->i1 net:\n\tMax: %d, Min: %d, Num: %d, countMax: %ld, countMin: %ld, edgesNum: %ld\n", maxId, minId, idNum, countMax, countMin, edgesNum); fflush(stdout);
	}

	return Bip;
}

//renew maxId/minId/countMax/countMin/idNum/edgesNum from count 
//count and id is always good&right.
static void renew_Bip2(struct Bip2 *bip) {
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
	for (i=bip->minId; i< bip->maxId + 1; ++i) {
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
//e.g. you get 5000 lines from the first calling of abstract_Bip2, but maybe you can only get
//4990 lines from the second calling. that means in the origin bip, there are 10 id whose count is 1.
//get it?
struct iiLineFile *abstract_Bip2(struct Bip2 *bip) {

	int i1Max=bip->maxId;
	int i1Min=bip->minId;
	int i2Max=-1;
	int i2Min=INT_MAX;
	
	int linesNum = bip->idNum;
	struct iiLine *lines = malloc(linesNum*sizeof(struct iiLine));
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
			i2Max = i2Max>lines[j].i2?i2Max:lines[j].i2;
			i2Min = i2Min<lines[j].i2?i2Max:lines[j].i2;
			++j;
			if (random == (degree - 1)) {
				--bip->count[i];
				if (bip->count[i] == 0) {
					//printf("%d, ", i);
					free(bip->id[i]);
					++k;
				}
			}
			else {
				bip->id[i][random] = bip->id[i][degree-1];
				--bip->count[i];
			}
		}
	}

	int origin = bip->idNum;
	renew_Bip2(bip);

	struct iiLineFile *file = malloc(sizeof(struct iiLineFile));
	assert(file != NULL);
	file->i1Max = i1Max;
	file->i1Min = i1Min;
	file->i2Max = i2Max;
	file->i2Min = i2Min;
	file->linesNum = j;
	file->lines = lines;
	printf("abstract_Bip2 done:\n\tBip2 originally has %d ids.\n\tabstract %d edges from %d.\n", origin, j, linesNum);
	printf("\tthere are %d ids whose degree has become 0 after abstract.\n", k);
	printf("\tNow Bip2 has %d ids.\n", bip->idNum);
	return file;
}

//create perfect iiLineFile
struct iiLineFile *backtofile_Bip2(struct Bip2 *bip) {
	int i1Max=bip->maxId;
	int i1Min=bip->minId;
	int i2Max=-1;
	int i2Min=INT_MAX;

	int linesNum = bip->edgesNum;
	struct iiLine *lines = malloc(linesNum*sizeof(struct iiLine));
	assert(lines != NULL);

	int i;
	int j=0;
	int k=0;
	for(i=0; i<i1Max+1; ++i) {
		if (bip->count[i] > 0) {
			for (j=0; j< bip->count[i]; ++j) {
				lines[k].i1 = i;
				lines[k].i2 = bip->id[i][j];
				i2Max = i2Max>lines[k].i2?i2Max:lines[k].i2;
				i2Min = i2Min<lines[k].i2?i2Max:lines[k].i2;
				++k;
			}
		}
	}

	struct iiLineFile *file = malloc(sizeof(struct iiLineFile));
	assert(file != NULL);
	file->i1Max = i1Max;
	file->i1Min = i1Min;
	file->i2Max = i2Max;
	file->i2Min = i2Min;
	file->linesNum = k;
	file->lines = lines;
	printf("backtofile_Bip2 done. %d lines generated.\n", k);
	return file;
}

// if count is 20, then the id whose count is 0-19 will be deleted.
void cutcount_Bip2(struct Bip2 *bip, long count) {
	if (count > bip->countMax || count < 0) {
		printf("cutcount_Bip2 error: wrong count.\n");
		return;
	}
	int i, j=0;
	for (i=0; i<bip->maxId + 1; ++i) {
		if (bip->count[i] > 0 && bip->count[i] < count) {
			bip->count[i] = 0;
			free(bip->id[i]);
			++j;
		}
	}
	int origin = bip->idNum;
	renew_Bip2(bip);
	printf("cutcount_Bip2 done:\n\tBip2 originally has %d ids.\n\tthere are %d ids whose count < %ld being deleted.\n\tNow Bip2 has %d ids.\n", origin, j, count, bip->idNum);fflush(stdout);
}

//calculate recovery of deleted links.
double recovery_Bip2_2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1) {
	double Rank = 0;
	int count = 0;

	double *i1source = malloc((bipi1->maxId + 1)*sizeof(double));
	assert(i1source != NULL);
	int *i1id = malloc((bipi1->maxId + 1)*sizeof(int));
	assert(i1id != NULL);
	char *i1sign = malloc((bipi1->maxId + 1)*sizeof(char));
	assert(i1sign != NULL);
	int i1affectedNum = 0;
	
	double *i2source = malloc((bipi2->maxId + 1)*sizeof(double));
	assert(i2source != NULL);
	int *i2id = malloc((bipi2->maxId + 1)*sizeof(int));
	assert(i2id != NULL);
	char *i2sign = malloc((bipi2->maxId + 1)*sizeof(char));
	assert(i2sign != NULL);
	int i2affectedNum = 0;

	double *ranksource = malloc((bipi2->maxId + 1)*sizeof(double));
	assert(ranksource != NULL);
	int *rank = calloc((bipi2->maxId + 1),sizeof(int));
	assert(rank != NULL);

	int i, i1;
	long j, degree;
	double source;
	int neigh, id;

	for (i1 = 0; i1<bipi1->maxId + 1; ++i1) { //each user
		if (i1%1000 ==0) {printf("%d\n", i1);fflush(stdout);}
		if (bipi1->count[i1] > 0 && testi1->count[i1] > 0) {
			count += testi1->count[i1];

			memset(i2source, 0, (bipi2->maxId+1)*sizeof(double));
			memset(i2sign, 0, (bipi2->maxId+1)*sizeof(char));
			memset(i2id, 0, (bipi2->maxId+1)*sizeof(int));
			i2affectedNum = 0;
			for (j=0; j<bipi1->count[i1]; ++j) {
				neigh = bipi1->id[i1][j];
				i2id[i2affectedNum] = neigh;
				i2sign[neigh] = 1;
				i2source[neigh] = 1;
				++i2affectedNum;
			}

			memset(i1source, 0, (bipi1->maxId+1)*sizeof(double));
			memset(i1sign, 0, (bipi1->maxId+1)*sizeof(char));
			memset(i1id, 0, (bipi1->maxId+1)*sizeof(int));
			i1affectedNum = 0;
			for (i=0; i<i2affectedNum; ++i) {
				id = i2id[i];
				degree = bipi2->count[id];
				source = i2source[id]/(double)degree;
				for (j=0; j<degree; ++j) {
					neigh = bipi2->id[id][j];
					if (!i1sign[neigh]) {
						i1id[i1affectedNum] = neigh;
						i1sign[neigh] = 1;
						++i1affectedNum;
					}
					i1source[neigh] += source;
				}
			}

			memset(i2source, 0, (bipi2->maxId+1)*sizeof(double));
			memset(i2sign, 0, (bipi2->maxId+1)*sizeof(char));
			memset(i2id, 0, (bipi2->maxId+1)*sizeof(int));
			i2affectedNum = 0;
			for (i=0; i<i1affectedNum; ++i) {
				id = i1id[i];
				degree = bipi1->count[id];
				source = (double)i1source[id]/(double)degree;
				for (j=0; j<degree; ++j) {
					neigh = bipi1->id[id][j];
					if (!i2sign[neigh]) {
						i2id[i2affectedNum] = neigh;
						i2sign[neigh] = 1;
						++i2affectedNum;
					}
					i2source[neigh] += source;
				}
			}
			

			//memset(ranksource, 0, (bipi2->maxId+1)*sizeof(double));
			//for (i=0; i<i2affectedNum; ++i) {
			//	ranksource[i] = i2source[i2id[i]];
			//}
			for (i=0; i<bipi2->maxId + 1; ++i) {
				i2id[i]=i;
			}

			qsort_di_desc(i2source, 0, bipi2->maxId, i2id);


			for (i=0; i<bipi2->maxId + 1; ++i) {
				rank[i] = i+1;
			}
			qsort_iid_asc(i2id, 0, bipi2->maxId, rank, ranksource);

			//for (i=0; i<i2affectedNum + 1; ++i) {
			//	printf("%d, %f, %d\n", i2id[i], ranksource[i], rank[i]);
			//}
			//return 0;

			double o_k = (double)(bipi2->idNum - bipi1->count[i1]);
			double tmp = 0.0;
			//for (i=i2affectedNum+1; i<bipi2->idNum + 1; ++i) {
			//	tmp += i;
			//}
			//double default_rankscore;
			//if (bipi2->idNum == i2affectedNum) {
			//	default_rankscore = 0;
			//}
			//else {
			//	default_rankscore = tmp / ((bipi2->idNum - i2affectedNum));
			//}
			//tmp = 0;
			for (j=0; j<testi1->count[i1]; ++j) {
				id = testi1->id[i1][j];
				//if (i2sign[id]) {
					tmp += rank[id];
				//}
				//else {
				//	tmp += default_rankscore;
				//}
			}
			tmp /= o_k;
			Rank += tmp;
		}
	}

	printf("%f\n", Rank/count);
	return Rank/count;
}

//calculate recovery of deleted links.
double recovery_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1) {
	double Rank = 0;
	int count = 0;

	double *i1source = calloc((bipi1->maxId + 1),sizeof(double));
	assert(i1source != NULL);
	
	double *i2source = calloc((bipi2->maxId + 1),sizeof(double));
	assert(i2source != NULL);

	int *rank = calloc((bipi2->maxId + 1),sizeof(int));
	assert(rank != NULL);
	int *i2id =  calloc((bipi2->maxId + 1),sizeof(int));
	assert(i2id != NULL);

	int i, i1;
	long j, degree;
	double source;
	int neigh, id;

	for (i1 = 0; i1<bipi1->maxId + 1; ++i1) { //each user
		if (i1%1000 ==0) {printf("%d\n", i1);fflush(stdout);}
		if (bipi1->count[i1] > 0 && testi1->count[i1] > 0) {
			count += testi1->count[i1];

			memset(i2source, 0, (bipi2->maxId+1)*sizeof(double));
			for (j=0; j<bipi1->count[i1]; ++j) {
				neigh = bipi1->id[i1][j];
				i2source[neigh] = 1;
			}

			memset(i1source, 0, (bipi1->maxId+1)*sizeof(double));
			for (i=0; i<bipi2->maxId + 1; ++i) {
				if (i2source[i]) {
					degree = bipi2->count[i];
					source = i2source[i]/(double)degree;
					for (j=0; j<degree; ++j) {
						neigh = bipi2->id[i][j];
						i1source[neigh] += source;
					}
				}
			}

			memset(i2source, 0, (bipi2->maxId+1)*sizeof(double));
			for (i=0; i<bipi1->maxId + 1; ++i) {
				if (i1source[i]) {
					degree = bipi1->count[i];
					source = (double)i1source[i]/(double)degree;
					for (j=0; j<degree; ++j) {
						neigh = bipi1->id[i][j];
						i2source[neigh] += source;
					}
				}
			}


			for (i=0; i<bipi2->maxId + 1; ++i) {
				i2id[i] = i;
			}

			qsort_di_desc(i2source, 0, bipi2->maxId, i2id);

			for (i=0; i<bipi2->maxId + 1; ++i) {
				rank[i] = i+1;
			}
			qsort_iid_asc(i2id, 0, bipi2->maxId, rank, i2source);

			//for (i=0; i<bipi2->maxId + 1; ++i) {
			//	if (i2source[i]) {
			//		printf("%d, %f, %d\n", i2id[i], i2source[i], rank[i]);
			//	}
			//}
			//exit(0);
			double o_k = (double)(bipi2->idNum - bipi1->count[i1]);
			double tmp = 0.0;
			for (j=0; j<testi1->count[i1]; ++j) {
				id = testi1->id[i1][j];
				tmp += rank[id];
			}
			tmp /= o_k;
			//tmp /= testi1->count[i1];
			//printf("%f\n", tmp);
			Rank += tmp;
		}
	}
	if (count == testi1->edgesNum) {
		printf("ddfadf\n");
	}
	printf("%f\n", Rank/testi1->edgesNum);
	return Rank/count;
}
