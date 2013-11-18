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

static void metrics_Bip2(int i1, struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, int L, int *rank, double *R, double *PL, double *IL, double *NL) {
			int unselected_list_length = bipi2->idNum - bipi1->count[i1];
			int rank_i1_j = 0;
			int DiL = 0;
			int j, id;
			for (j=0; j<testi1->count[i1]; ++j) {
				id = testi1->id[i1][j];
				rank_i1_j += rank[id];
				if (rank[id] < L) {
					++DiL;
				}
			}
			*R += (double)rank_i1_j/(double)unselected_list_length;
			*PL += (double)DiL/(double)L;
}

static double metrics_HL_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, int L, int *Hij) {
	int *sign = calloc((bipi2->maxId + 1), sizeof(int));
	assert(sign != NULL);
	int i, j;
	long k;
	int cou = 0;
	int Cij = 0;
	double HL = 0;
	for (i=3; i<bipi1->maxId + 1; ++i) {
		if (bipi1->count[i] && testi1->count[i]) {
			memset(sign, 0, (bipi2->maxId + 1)*sizeof(int));
			for (k=i*L; k<i*L+L; ++k) {
				sign[Hij[k]] = 1;
			}
			for (j=i+1; j<bipi1->maxId + 1; ++j) {
				if (bipi1->count[j] && testi1->count[j]) {
					Cij = 0;
					for (k=j*L; k<j*L+L; ++k) {
						if (sign[Hij[k]]) {
							++Cij;
						}
					}
					HL += 1 - ((double)Cij)/(double)L;
					//printf("%d %d %d\n", i, j, Cij);
					++cou;
				}

			}
		}
	}
	printf("pairs of users: %d, %f\n", cou, HL);
	return HL/cou;
}

static void recovery_probs_Bip2_core(int i1, struct Bip2 *bipi1, struct Bip2 *bipi2, double *i1source, double *i2source, int L, int *i2id, int *rank, int *Hij) {
	int i, j, neigh;
	long degree;
	double source;
	memset(i2source, 0, (bipi2->maxId+1)*sizeof(double));
	for (j=0; j<bipi1->count[i1]; ++j) {
		neigh = bipi1->id[i1][j];
		i2source[neigh] = 1.0;
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
	for (i=0; i<bipi1->count[i1]; ++i) {
		i2source[bipi1->id[i1][i]] = 0;
	}

	for (i=0; i<bipi2->maxId + 1; ++i) {
		i2id[i] = i;
		rank[i] = i+1;
	}
	qsort_di_desc(i2source, 0, bipi2->maxId, i2id);
	memcpy(Hij+i1*L, i2id, L*sizeof(int));
	qsort_iid_asc(i2id, 0, bipi2->maxId, rank, i2source);
}

//calculate recovery of deleted links.
double recovery_probs_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2) {
	//the metrics needed to be calculated.
	double R, PL, HL, IL, NL;
	R=PL=HL=IL=NL=0;

	double *i1source = calloc((bipi1->maxId + 1),sizeof(double));
	assert(i1source != NULL);
	double *i2source = calloc((bipi2->maxId + 1),sizeof(double));
	assert(i2source != NULL);
	int *rank = calloc((bipi2->maxId + 1),sizeof(int));
	assert(rank != NULL);
	int *i2id =  calloc((bipi2->maxId + 1),sizeof(int));
	assert(i2id != NULL);

	int i;
	int L = 50;
	int *Hij = malloc(50*(bipi1->maxId + 1)*sizeof(int));
	assert(Hij != NULL);
	for (i = 0; i<bipi1->maxId + 1; ++i) { //each user
		if (i%1000 ==0) {printf("%d\n", i);fflush(stdout);}
		//only compute the i in both bipi1 and testi1.
		if (bipi1->count[i] && testi1->count[i]) {
			//get rank
			recovery_probs_Bip2_core(i, bipi1, bipi2, i1source, i2source, L, i2id, rank, Hij);
			//use rank to get metrics values
			metrics_Bip2(i, bipi1, bipi2, testi1, testi2, L, rank, &R, &PL, &IL, &NL);
		}
	}
	//int j;
	//for (i = 0; i<bipi1->maxId + 1; ++i) { //each user
	//	for (j=0; j<50; ++j) {
	//		printf("%d, %d, %d\n", i, j, Hij[i*50 + j]);
	//	}
	//}

	R /= testi1->edgesNum;
	PL /= testi1->idNum;
	HL = metrics_HL_Bip2(bipi1, bipi2, testi1, L, Hij);

	printf("R: %f, PL: %f, HL: %f\n", R, PL, HL);
	free(i1source);
	free(i2source);
	free(i2id);
	free(rank);
	return 0;
}

static void recovery_heats_Bip2_core(int i1, struct Bip2 *bipi1, struct Bip2 *bipi2, double *i1source, double *i2source, int *i2id, int *rank) {
	int neigh, i;
	double source;
	long j, degree;

			memset(i2source, 0, (bipi2->maxId+1)*sizeof(double));
			for (j=0; j<bipi1->count[i1]; ++j) {
				neigh = bipi1->id[i1][j];
				i2source[neigh] = 1;
			}

			memset(i1source, 0, (bipi1->maxId+1)*sizeof(double));
			for (i=0; i<bipi2->maxId + 1; ++i) {
				if (i2source[i]) {
					for (j=0; j<bipi2->count[i]; ++j) {
						neigh = bipi2->id[i][j];
						i1source[neigh] += i2source[i];
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
				if (i2source[i]) {
					i2source[i] /= bipi2->count[i];
				}
			}
			for (i=0; i<bipi1->count[i1]; ++i) {
				i2source[bipi1->id[i1][i]] = 0;
			}

			for (i=0; i<bipi2->maxId + 1; ++i) {
				i2id[i] = i;
				rank[i] = i+1;
			}
			qsort_di_desc(i2source, 0, bipi2->maxId, i2id);
			qsort_iid_asc(i2id, 0, bipi2->maxId, rank, i2source);
}

double recovery_heats_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2) {
	double R, PL, HL, IL, NL;
	R=PL=HL=IL=NL=0;

	double *i1source = calloc((bipi1->maxId + 1),sizeof(double));
	assert(i1source != NULL);
	double *i2source = calloc((bipi2->maxId + 1),sizeof(double));
	assert(i2source != NULL);
	int *rank = calloc((bipi2->maxId + 1),sizeof(int));
	assert(rank != NULL);
	int *i2id =  calloc((bipi2->maxId + 1),sizeof(int));
	assert(i2id != NULL);

	int i1;
	int L = 50;
	for (i1 = 0; i1<bipi1->maxId + 1; ++i1) { //each user
		if (i1%1000 ==0) {printf("%d\n", i1);fflush(stdout);}
		if (bipi1->count[i1] > 0 && testi1->count[i1] > 0) {
			recovery_heats_Bip2_core(i1, bipi1, bipi2, i1source, i2source, i2id, rank);
			metrics_Bip2(i1, bipi1, bipi2, testi1, testi2, L, rank, &R, &PL, &IL, &NL);

			//for (i=0; i<bipi2->maxId + 1; ++i) {
			//	if (bipi2->count[i]) {
			//		printf("%d, %f, %d\n", i2id[i], i2source[i], rank[i]);
			//	}
			//}
			//exit(0);
		}
	}
	R /= testi1->edgesNum;
	PL /= testi1->idNum;
	printf("%f\n", R);
	printf("%f\n", PL);

	free(i1source);
	free(i2source);
	free(i2id);
	free(rank);
	return 0;
}

double recovery_grank_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1) {
	double Rank = 0;
	double Ep = 0;
	int count = 0;

	int *rank = calloc((bipi2->maxId + 1),sizeof(int));
	assert(rank != NULL);
	int *i2id =  calloc((bipi2->maxId + 1),sizeof(int));
	assert(i2id != NULL);

	int *degree = malloc((bipi2->maxId + 1)*sizeof(int));;	
	assert(degree != NULL);

	int i, j, i1, id;


			//for (i=0; i<bipi2->maxId + 1; ++i) {
			//	if (bipi2->count[i]) {
			//		printf("%d, %f, %d\n", i2id[i], i2source[i], rank[i]);
			//	}
			//}
			//exit(0);
	for (i1=0; i1<bipi1->maxId + 1; ++i1) {
		if(bipi1->count[i1]) {
			for (i=0; i<bipi2->maxId + 1; ++i) {
				i2id[i] = i;
				degree[i] = bipi2->count[i];
				rank[i] = i+1;
			}
			for (j=0; j<bipi1->count[i1]; ++j) {
				degree[bipi1->id[i1][j]] = 0;
			}
			qsort_ii_desc(degree, 0, bipi2->maxId, i2id);
			qsort_i3_asc(i2id, 0, bipi2->maxId, rank, degree);
			++count;
			int o_k = bipi2->idNum - bipi1->count[i1];
			int tmp = 0;
			int L = 20;
			int DiL = 0;			
			for (j=0; j<testi1->count[i1]; ++j) {
				id = testi1->id[i1][j];
				tmp += rank[id];
				if (rank[id] < L) {
					++DiL;
				}
			}
			double PL = (double)DiL/(double)L;
			//double RL = (double)DiL/(double)testi1->count[i1];
			//tmp /= o_k;
			//tmp /= testi1->count[i1];
			//printf("%f\n", tmp);
			Rank += (double)tmp/(double)o_k;
			Ep += PL;
		}
	}
	Ep /= count;
	Ep = Ep*bipi2->idNum*bipi1->idNum/testi1->edgesNum;
	printf("%f\n", Rank/testi1->edgesNum);
	printf("%f\n", Ep);

	free(i2id);
	free(rank);
	free(degree);

	return Rank/count;
}


void *verifyBip2(struct Bip2 *bipi1, struct Bip2 *bipi2) {
	long i;
	int j,k;
	int *place = malloc((bipi2->maxId+1)*sizeof(int));
	FILE *fp = fopen("data/duplicatePairsinNet", "w");
	fileError(fp, "data/duplicatePairsinNet");
	FILE *fp2 = fopen("data/NoDuplicatePairsNetFile", "w");
	fileError(fp2, "data/NoDuplicatePairsNetFile");
	fprintf(fp, "the following pairs are duplicate in the net file\n");
	char sign=0;
	for (j=0; j<bipi1->maxId+1; ++j) {
		if (bipi1->count[j]>0) {
			for (k=0; k<bipi2->maxId + 1; ++k) {
				place[k] = -1;
			}
			for (i=0; i<bipi1->count[j]; ++i) {
				int origin = bipi1->id[j][i];
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
		if (j%10000 == 0) {
			printf("%d\n", j);
			fflush(stdout);
		}
	}
	free(place);
	fclose(fp);
	fclose(fp2);
	if (sign == 1) {
		isError("the file has duplicate pairs, you can check data/duplicatePairsinNet.\nwe generate a net file named data/NoDuplicatePairsNetFile which doesn't contain any duplicate pairsr.\nyou should use this file instead the origin wrong one.\n");
	}
	else {
		printf("verifyBip2: perfect network.\n");
	}
	return (void *)0;
}

struct iiLineFile *divide_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, double rate) {
	if (rate <=0 || rate >= 1) {
		printf("divide_Bip2 error: wrong rate.\n");
		return NULL;
	}
	long l1, l2;
	if (bipi1->edgesNum > 100000) {
		l1 = (int)(bipi1->edgesNum*(rate+0.1));
		l2 = (int)(bipi1->edgesNum*(1-rate+0.1));
	}
	else {
		l2 = l1 = bipi1->edgesNum;
	}

	struct iiLineFile *twofile = malloc(2*sizeof(struct iiLineFile));
	assert(twofile != NULL);

	twofile[0].lines = malloc(l1*sizeof(struct iiLine));
	assert(twofile[0].lines != NULL);
	twofile[1].lines = malloc(l2*sizeof(struct iiLine));
	assert(twofile[1].lines != NULL);

	long line1=0, line2=0;
	int i1Max=-1; 
	int i2Max=-1;
    int i1Min=INT_MAX;
    int i2Min=INT_MAX;
	int _i1Max=-1; 
	int _i2Max=-1;
    int _i1Min=INT_MAX;
    int _i2Min=INT_MAX;

	char *i1sign = calloc(bipi1->maxId + 1, sizeof(char));
	assert(i1sign);
	char *i2sign = calloc(bipi1->maxId + 1, sizeof(char));
	assert(i2sign);

	long *counti1 = malloc((bipi1->maxId + 1)*sizeof(long));
	assert(counti1 != NULL);
	memcpy(counti1, bipi1->count, (bipi1->maxId + 1)*sizeof(long));
	long *counti2 = malloc((bipi2->maxId + 1)*sizeof(long));
	assert(counti2 != NULL);
	memcpy(counti2, bipi2->count, (bipi2->maxId + 1)*sizeof(long));

	int i, neigh;
	long j;
	int i1 = bipi1->minId, i2 = bipi2->minId;
	for (i=0; i<bipi1->maxId + 1; ++i) {
		for (j=0; j<bipi1->count[i]; ++j) {
			neigh = bipi1->id[i][j];
			//if (i == i1 || neigh == i2) {
			//	twofile[1].lines[line2].i1 = i;	
			//	twofile[1].lines[line2].i2 = neigh;	
			//	if (i == i1) ++i1;
			//	if (neigh == i2) ++i2;
			//	_i1Max = _i1Max>i?_i1Max:i;
			//	_i2Max = _i2Max>neigh?_i2Max:neigh;
			//	_i1Min = _i1Min<i?_i1Min:i;
			//	_i2Min = _i2Min<neigh?_i2Min:neigh;
			//	++line2;
			//}
			//else 
			if (genrand_real1() < rate) {
				if ((counti1[i] == 1 && i1sign[i] == 0) || (counti2[neigh] == 1 && i2sign[neigh] == 0)) {
					twofile[1].lines[line2].i1 = i;	
					twofile[1].lines[line2].i2 = neigh;	
					--counti1[i];
					--counti2[neigh];
					i1sign[i] = 1;
					i2sign[neigh] = 1;
					_i1Max = _i1Max>i?_i1Max:i;
					_i2Max = _i2Max>neigh?_i2Max:neigh;
					_i1Min = _i1Min<i?_i1Min:i;
					_i2Min = _i2Min<neigh?_i2Min:neigh;
					++line2;
				}
				twofile[0].lines[line1].i1 = i;	
				twofile[0].lines[line1].i2 = neigh;	
				--counti1[i];
				--counti2[neigh];
				i1Max = i1Max>i?i1Max:i;
				i2Max = i2Max>neigh?i2Max:neigh;
				i1Min = i1Min<i?i1Min:i;
				i2Min = i2Min<neigh?i2Min:neigh;
				++line1;
			}
			else {
				twofile[1].lines[line2].i1 = i;	
				twofile[1].lines[line2].i2 = neigh;	
				i1sign[i] = 1;
				i2sign[neigh] = 1;
				--counti1[i];
				--counti2[neigh];
				_i1Max = _i1Max>i?_i1Max:i;
				_i2Max = _i2Max>neigh?_i2Max:neigh;
				_i1Min = _i1Min<i?_i1Min:i;
				_i2Min = _i2Min<neigh?_i2Min:neigh;
				++line2;
			}
		}
	}
	assert((line1 <= l1) && (line2 <= l2));

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
	printf("divide_Bip2 done:\n\trate: %f\n\tfile1: linesNum: %ld, i1Max: %d, i1Min: %d, i2Max: %d, i2Min: %d\n\tfile2: linesNum: %ld, i1Max: %d, i1Min: %d, i2Max: %d, i2Min: %d\n", rate, line1, i1Max, i1Min, i2Max, i2Min, line2, _i1Max, _i1Min, _i2Max, _i2Min);fflush(stdout);
	return twofile;
}
