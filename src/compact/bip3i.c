#include "../../inc/compact/bip3i.h"
#include "../../inc/utility/error.h"
#include "../../inc/utility/random.h"
#include "../../inc/utility/sort.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

//struct Bip3i {
//	int maxId;
//	int minId;
//	int idNum;
//	long countMax;
//	long countMin;
//	long *count;
//	int **id;
//	int **i3;
//	long edgesNum;
//};

struct L_Bip3i *create_L_Bip3i(void) {
	struct L_Bip3i *lp = malloc(sizeof(struct L_Bip3i));
	assert(lp != NULL);
	lp->R = 0;
	lp->PL = 0;
	lp->HL = 0;
	lp->IL = 0;
	lp->NL = 0;
	lp->L = 0;
	lp->topL = NULL;
	return lp;
}
//free(NULL) is ok.
void clean_L_Bip3i(struct L_Bip3i *lp) {
	lp->R = 0;
	lp->PL = 0;
	lp->HL = 0;
	lp->IL = 0;
	lp->NL = 0;
	lp->L = 0;
	free(lp->topL);
	lp->topL = NULL;
}
void free_L_Bip3i(struct L_Bip3i *lp) {
	free(lp->topL);
	free(lp);
}

struct param_recommend_Bip3i {
	double theta;
	double eta;
	double epsilon;
	double lambda;
	int maxscore;
};

void free_Bip3i(struct Bip3i *Bip) {
	int i=0;
	for(i=0; i<Bip->maxId+1; ++i) {
		if (Bip->count[i]>0) {
			free(Bip->id[i]);
			free(Bip->i3[i]);
		}
	}
	free(Bip->count);
	free(Bip->id);
	free(Bip->i3);
	free(Bip);
}

struct Bip3i *create_Bip3i(const struct i3LineFile * const file, int i1toi2) {
	//all elements of struct Bip3i.
	int maxId;
	int minId;
	int idNum=0;
	long countMax=-1;
	long countMin=LONG_MAX;
	long *count;
	int **id;
	int **i3;
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

	//fill the count.
	edgesNum=file->linesNum;
	struct i3Line *lines=file->lines;
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
		}
		else {
			id[j] = NULL;
			i3[j] = NULL;
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
			++temp[i1];
		}
	}
	else {
		for(i=0; i<edgesNum; ++i) {
			int i2 =lines[i].i2;
			id[i2][temp[i2]]=lines[i].i1;
			i3[i2][temp[i2]]=lines[i].i3;
			++temp[i2];
		}
	}
	free(temp);

	//fill Bip and return.
	struct Bip3i *Bip = malloc(sizeof(struct Bip3i));
	assert(Bip != NULL);

	Bip->maxId=maxId;
	Bip->minId=minId;
	Bip->idNum=idNum;
	Bip->countMax = countMax;
	Bip->countMin = countMin;
	Bip->count=count;
	Bip->id = id;
	Bip->i3 = i3;
	Bip->edgesNum=edgesNum;

	if (i1toi2) {
		printf("build i1->i2 net:\n\tMax: %d, Min: %d, Num: %d, countMax: %ld, countMin: %ld, edgesNum: %ld\n", maxId, minId, idNum, countMax, countMin, edgesNum); fflush(stdout);
	}
	else {
		printf("build i2->i1 net:\n\tMax: %d, Min: %d, Num: %d, countMax: %ld, countMin: %ld, edgesNum: %ld\n", maxId, minId, idNum, countMax, countMin, edgesNum); fflush(stdout);
	}

	return Bip;
}

void *verifyBip3i(struct Bip3i *bipi1, struct Bip3i *bipi2) {
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
					fprintf(fp2, "%d, %d, %d\n", j, origin, bipi1->i3[j][i]);
				}
				else {
					fprintf(fp, "%d, %d, %d\n", j, next, bipi1->i3[j][i]);
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
		printf("verifyBip3i: perfect network.\n");
	}
	return (void *)0;
}

//divide Bip3i into two parts.
//return two struct i3LineFile. the first one is always the small one.
//the second is always the large one.
struct i3LineFile *divide_Bip3i(struct Bip3i *bipi1, struct Bip3i *bipi2, double rate) {
	if (rate <=0 || rate >= 1) {
		printf("divide_Bip3i error: wrong rate.\n");
		return NULL;
	}
	rate = rate>0.5?1-rate:rate;
	long l1, l2;
	if (bipi1->edgesNum > 100000) {
		l1 = (int)(bipi1->edgesNum*(rate+0.1));
		l2 = (int)(bipi1->edgesNum*(1-rate+0.1));
	}
	else {
		l2 = l1 = bipi1->edgesNum;
	}

	struct i3LineFile *twofile = malloc(2*sizeof(struct i3LineFile));
	assert(twofile != NULL);

	twofile[0].lines = malloc(l1*sizeof(struct i3Line));
	assert(twofile[0].lines != NULL);
	twofile[1].lines = malloc(l2*sizeof(struct i3Line));
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
	char *i2sign = calloc(bipi2->maxId + 1, sizeof(char));
	assert(i2sign);

	long *counti1 = malloc((bipi1->maxId + 1)*sizeof(long));
	assert(counti1 != NULL);
	memcpy(counti1, bipi1->count, (bipi1->maxId + 1)*sizeof(long));
	long *counti2 = malloc((bipi2->maxId + 1)*sizeof(long));
	assert(counti2 != NULL);
	memcpy(counti2, bipi2->count, (bipi2->maxId + 1)*sizeof(long));

	int i, neigh, score;
	long j;
	for (i=0; i<bipi1->maxId + 1; ++i) {
		for (j=0; j<bipi1->count[i]; ++j) {
			neigh = bipi1->id[i][j];
			score = bipi1->i3[i][j];
			if (genrand_real1() < rate) {
				if ((counti1[i] == 1 && i1sign[i] == 0) || (counti2[neigh] == 1 && i2sign[neigh] == 0)) {
					twofile[1].lines[line2].i1 = i;	
					twofile[1].lines[line2].i2 = neigh;	
					twofile[1].lines[line2].i3 = score;	
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
				else {
					twofile[0].lines[line1].i1 = i;	
					twofile[0].lines[line1].i2 = neigh;	
					twofile[0].lines[line1].i3 = score;	
					--counti1[i];
					--counti2[neigh];
					i1Max = i1Max>i?i1Max:i;
					i2Max = i2Max>neigh?i2Max:neigh;
					i1Min = i1Min<i?i1Min:i;
					i2Min = i2Min<neigh?i2Min:neigh;
					++line1;
				}
			}
			else {
				twofile[1].lines[line2].i1 = i;	
				twofile[1].lines[line2].i2 = neigh;	
				twofile[1].lines[line2].i3 = score;	
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

	free(i1sign);
	free(i2sign);
	free(counti1);
	free(counti2);

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
	printf("divide_Bip3i done:\n\trate: %f\n\tfile1: linesNum: %ld, i1Max: %d, i1Min: %d, i2Max: %d, i2Min: %d\n\tfile2: linesNum: %ld, i1Max: %d, i1Min: %d, i2Max: %d, i2Min: %d\n", rate, line1, i1Max, i1Min, i2Max, i2Min, line2, _i1Max, _i1Min, _i2Max, _i2Min);fflush(stdout);
	return twofile;
}

//following is for recommendation.
//Warning: remeber the maxId in testset maybe smaller than the maxId in trainset.
//R is rankscore.
//PL is precision
//Warning: about unselected_list_length, I use traini2->maxId, not traini2->idNum. this actually is wrong I think, but it's the way linyulv did.
static void metrics_Bip3i(int i1, struct Bip3i *traini1, struct Bip3i *traini2, struct Bip3i *testi1, int L, int *rank, double *R, double *PL) {
	if (i1<testi1->maxId + 1 &&  testi1->count[i1]) {
		//int unselected_list_length = traini2->idNum - traini1->count[i1];
		int unselected_list_length = traini2->maxId - traini1->count[i1];
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
}
//IL is intrasimilarity
static double metrics_IL_Bip3i(struct Bip3i *traini1, struct Bip3i *traini2, struct Bip3i *testi1, int L, int *Hij, struct iidNet *sim) {
	if (!sim) return -1;
	double *sign = calloc((traini2->maxId + 1), sizeof(double));
	assert(sign != NULL);
	int i, j;
	long k;
	double IL = 0;
	int cou = 0;
	for (i=0; i<traini1->maxId + 1; ++i) {
		if (traini1->count[i]) {
			++cou;
			int *tmp = Hij + i*L;
			for (j=0; j<L; ++j) {
				int id = tmp[j];
				memset(sign, 0, (traini2->maxId + 1)*sizeof(double));
				for (k=0; k<sim->count[id]; ++k) {
					sign[sim->edges[id][k]] = sim->d3[id][k];
				}
				for (k=j+1; k<L; ++k) {
					id = tmp[k];
					IL += sign[id];
				}
			}
		}
	}
	free(sign);
	IL /= L*(L-1)*cou;
	return 2*IL;
}
//HL is hamming distance.
static double metrics_HL_Bip3i(struct Bip3i *traini1, struct Bip3i *traini2, struct Bip3i *testi1, int L, int *Hij) {
	int *sign = calloc((traini2->maxId + 1), sizeof(int));
	assert(sign != NULL);
	int i, j;
	long k;
	int cou = 0;
	int Cij = 0;
	double HL = 0;
	for (i=0; i<traini1->maxId + 1; ++i) {
		if (traini1->count[i]) {
			memset(sign, 0, (traini2->maxId + 1)*sizeof(int));
			for (k=i*L; k<i*L+L; ++k) {
				sign[Hij[k]] = 1;
			}
			for (j=i+1; j<traini1->maxId + 1; ++j) {
				if (traini1->count[j] && testi1->count[j]) {
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
	free(sign);
	return HL/cou;
}
//NL is popularity.
static double metrics_NL_Bip3i(struct Bip3i *traini1, struct Bip3i *traini2, struct Bip3i *testi1, int L, int *Hij) {
	int i,j;
	long NL = 0;
	int cou = 0;
	for (i=0; i<traini1->maxId + 1; ++i) {
		if (traini1->count[i]) {
			++cou;
			int *tmp = Hij + i*L;
			for (j=0; j<L; ++j) {
				NL += traini2->count[tmp[j]];
			}
		}
	}
	NL /= L*cou;
	return NL;
}
//three-step random walk of Probs
static void s_mass_Bip3i_core(int i1, struct Bip3i *traini1, struct Bip3i *traini2, double *i1source, double *i2source, double *i1sourceA, double *i2sourceA, int L, int *i2id, int *rank, int *topL, double theta, int maxscore) {
	int i, j, neigh;
	long degree;
	double source;
	//one 
	double totalsource = 0;
	memset(i2id, 0, (traini2->maxId +1)*sizeof(int));
	memset(i2source, 0, (traini2->maxId+1)*sizeof(double));
	for (j=0; j<traini1->count[i1]; ++j) {
		neigh = traini1->id[i1][j];
		i2id[neigh] = traini1->i3[i1][j];
		i2source[neigh] = pow(traini1->i3[i1][j], theta);
		totalsource += i2source[neigh];	
	}
	for (j=0; j<traini1->count[i1]; ++j) {
		i2source[neigh] = i2source[neigh]*traini1->count[i1]/totalsource;
	}
	//two
	memset(i1source, 0, (traini1->maxId+1)*sizeof(double));
	//memset(i1sourceA, 0, (traini1->maxId+1)*sizeof(double));
	for (i=0; i<traini2->maxId + 1; ++i) {
		if (i2source[i]) {
			degree = traini2->count[i];
			source = i2source[i];
			totalsource = 0;
			for (j=0; j<degree; ++j) {
				neigh = traini2->id[i][j];
				i1sourceA[neigh] = pow(5 - fabs(traini2->i3[i][j]-i2id[i]), theta); 
				//i1source[neigh] = source*source2;
				totalsource += i1sourceA[neigh];
			}
			for (j=0; j<degree; ++j) {
				neigh = traini2->id[i][j];
				i1source[neigh] += source*i1sourceA[neigh]/totalsource;
			}
		}
	}
	//three
	memset(i2source, 0, (traini2->maxId+1)*sizeof(double));
	for (i=0; i<traini1->maxId + 1; ++i) {
		if (i1source[i]) {
			totalsource = 0;
			degree = traini1->count[i];
			source = i1source[i];
			for (j=0; j<degree; ++j) {
				neigh = traini1->id[i][j];
				i2sourceA[neigh] = pow((double)traini1->i3[i][j]/(double)traini2->count[neigh], theta);
				totalsource += i2sourceA[neigh];
			}
			for (j=0; j<degree; ++j) {
				neigh = traini1->id[i][j];
				i2source[neigh] += source*i2sourceA[neigh]/totalsource;
			}
		}
	}
	//double t=0;
	//for (i=0; i<traini2->maxId + 1; ++i) {
	//	t += i2source[i];
	//}
	//printf("%d, %ld, %f\n", i1, traini1->count[i1], t);
	//set i2id and rank.
	for (i=0; i<traini1->count[i1]; ++i) {
		i2source[traini1->id[i1][i]] = 0;
	}
	for (i=0; i<traini2->maxId + 1; ++i) {
		i2id[i] = i;
		rank[i] = i+1;
	}
	//after qsort_di_desc, the id of the item with most source will be in i2id[0];
	qsort_di_desc(i2source, 0, traini2->maxId, i2id);
	//copy the top L itemid into topL.
	memcpy(topL+i1*L, i2id, L*sizeof(int));
	//after qsort_iid_asc, the rank of the item whose id is x will be in rank[x];
	qsort_iid_asc(i2id, 0, traini2->maxId, rank, i2source);
}

static void d_mass_Bip3i_core(int i1, struct Bip3i *traini1, struct Bip3i *traini2, double *i1source, double *i2source, double *i2sourceA, int L, int *i2id, int *rank, int *topL, double eta) {
	int i, j, neigh;
	long degree;
	double source;
	//one 
	memset(i2source, 0, (traini2->maxId+1)*sizeof(double));
	for (j=0; j<traini1->count[i1]; ++j) {
		neigh = traini1->id[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (traini1->maxId+1)*sizeof(double));
	for (i=0; i<traini2->maxId + 1; ++i) {
		if (i2source[i]) {
			degree = traini2->count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = traini2->id[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//three
	double totalsource;
	memset(i2source, 0, (traini2->maxId+1)*sizeof(double));
	for (i=0; i<traini1->maxId + 1; ++i) {
		if (i1source[i]) {
			totalsource = 0;
			degree = traini1->count[i];
			source = i1source[i];
			for (j=0; j<degree; ++j) {
				neigh = traini1->id[i][j];
				i2sourceA[neigh] = pow(1.0/traini2->count[neigh], eta);
				totalsource += i2sourceA[neigh];
			}
			for (j=0; j<degree; ++j) {
				neigh = traini1->id[i][j];
				i2source[neigh] += source*i2sourceA[neigh]/totalsource;
			}
		}
	}
	//set selected item's source to 0
	for (i=0; i<traini1->count[i1]; ++i) {
		i2source[traini1->id[i1][i]] = 0;
	}
	//set i2id and rank.
	for (i=0; i<traini2->maxId + 1; ++i) {
		i2id[i] = i;
		rank[i] = i+1;
	}
	//after qsort_di_desc, the id of the item with most source will be in i2id[0];
	qsort_di_desc(i2source, 0, traini2->maxId, i2id);
	//copy the top L itemid into topL.
	memcpy(topL+i1*L, i2id, L*sizeof(int));
	//after qsort_iid_asc, the rank of the item whose id is x will be in rank[x];
	qsort_iid_asc(i2id, 0, traini2->maxId, rank, i2source);
}

static void thirdstepSD_mass_Bip3i_core(int i1, struct Bip3i *traini1, struct Bip3i *traini2, double *i1source, double *i2source, double *i2sourceA, int L, int *i2id, int *rank, int *topL, double epsilon, int maxscore) {
	int i, j, neigh;
	long degree;
	double source;
	//one 
	memset(i2source, 0, (traini2->maxId+1)*sizeof(double));
	for (j=0; j<traini1->count[i1]; ++j) {
		neigh = traini1->id[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (traini1->maxId+1)*sizeof(double));
	for (i=0; i<traini2->maxId + 1; ++i) {
		if (i2source[i]) {
			degree = traini2->count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = traini2->id[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//three
	double totalsource;
	memset(i2source, 0, (traini2->maxId+1)*sizeof(double));
	for (i=0; i<traini1->maxId + 1; ++i) {
		if (i1source[i]) {
			totalsource = 0;
			degree = traini1->count[i];
			source = i1source[i];
			for (j=0; j<degree; ++j) {
				neigh = traini1->id[i][j];
				i2sourceA[neigh] = pow((double)traini1->i3[i][j]/(double)traini2->count[neigh], epsilon);
				totalsource += i2sourceA[neigh];
			}
			for (j=0; j<degree; ++j) {
				neigh = traini1->id[i][j];
				i2source[neigh] += source*i2sourceA[neigh]/totalsource;
			}
		}
	}
	//set selected item's source to 0
	for (i=0; i<traini1->count[i1]; ++i) {
		i2source[traini1->id[i1][i]] = 0;
	}
	//set i2id and rank.
	for (i=0; i<traini2->maxId + 1; ++i) {
		i2id[i] = i;
		rank[i] = i+1;
	}
	//after qsort_di_desc, the id of the item with most source will be in i2id[0];
	qsort_di_desc(i2source, 0, traini2->maxId, i2id);
	//copy the top L itemid into topL.
	memcpy(topL+i1*L, i2id, L*sizeof(int));
	//after qsort_iid_asc, the rank of the item whose id is x will be in rank[x];
	qsort_iid_asc(i2id, 0, traini2->maxId, rank, i2source);
}
//three-step random walk of hybrid
static void hybrid_Bip3i_core(int i1, struct Bip3i *traini1, struct Bip3i *traini2, double *i1source, double *i2source, int L, int *i2id, int *rank, int *topL, double lambda) {
	int neigh, i;
	//double source;
	long j;
	//one
	memset(i2source, 0, (traini2->maxId+1)*sizeof(double));
	for (j=0; j<traini1->count[i1]; ++j) {
		neigh = traini1->id[i1][j];
		i2source[neigh] = 1;
	}
	//two
	memset(i1source, 0, (traini1->maxId+1)*sizeof(double));
	for (i=0; i<traini2->maxId + 1; ++i) {
		if (i2source[i]) {
			double powl = pow(traini2->count[i], lambda);
			for (j=0; j<traini2->count[i]; ++j) {
				neigh = traini2->id[i][j];
				i1source[neigh] += i2source[i]/powl;
			}
		}
	}
	//three
	memset(i2source, 0, (traini2->maxId+1)*sizeof(double));
	for (i=0; i<traini2->maxId + 1; ++i) {
		if (traini2->count[i]) {
			double powl = pow(traini2->count[i], 1-lambda);
			for (j=0; j<traini2->count[i]; ++j) {
				neigh = traini2->id[i][j];
				i2source[i] += i1source[neigh]/traini1->count[neigh];
			}
			i2source[i] /= powl;
		}
	}
	for (i=0; i<traini1->count[i1]; ++i) {
		i2source[traini1->id[i1][i]] = 0;
	}

	for (i=0; i<traini2->maxId + 1; ++i) {
		i2id[i] = i;
		rank[i] = i+1;
	}
	qsort_di_desc(i2source, 0, traini2->maxId, i2id);
	memcpy(topL+i1*L, i2id, L*sizeof(int));
	qsort_iid_asc(i2id, 0, traini2->maxId, rank, i2source);
}

/** 
 * core function of recommendation.
 * type :
 * 1 -- score degree mass (theta)
 * 2 -- degree mass (eta)
 * 3 -- only third step change, similar to 2, but with both score and degree. (epsilon)
 * 4 -- origin hybrid (lambda)
 *
 * all L is from this function. if you want to change, change the L below.
 */
static struct L_Bip3i *recommend_Bip3i(int type, struct Bip3i *traini1, struct Bip3i *traini2, struct Bip3i *testi1, struct Bip3i *testi2, struct iidNet *trainSim, struct param_recommend_Bip3i param) {
	double theta = param.theta;
	double eta = param.eta;
	double epsilon = param.epsilon;
	double lambda = param.lambda;
	int maxscore = param.maxscore;

	int L = 50;

	double R, PL, HL, IL, NL;
	R=PL=HL=IL=NL=0;

	double *i1source = malloc((traini1->maxId + 1)*sizeof(double));
	assert(i1source != NULL);
	double *i1sourceA = malloc((traini1->maxId + 1)*sizeof(double));
	assert(i1sourceA != NULL);
	double *i2source = malloc((traini2->maxId + 1)*sizeof(double));
	assert(i2source != NULL);
	double *i2sourceA = malloc((traini2->maxId + 1)*sizeof(double));
	assert(i2sourceA != NULL);
	int *rank = malloc((traini2->maxId + 1)*sizeof(int));
	assert(rank != NULL);
	int *i2id =  malloc((traini2->maxId + 1)*sizeof(int));
	assert(i2id != NULL);

	int i;
	int *topL = calloc(L*(traini1->maxId + 1), sizeof(int));
	assert(topL != NULL);
	switch (type) {
		case 1:
			for (i = 0; i<traini1->maxId + 1; ++i) { //each user
				//if (i%1000 ==0) {printf("%d\n", i);fflush(stdout);}
				if (traini1->count[i]) {
					//get rank
					s_mass_Bip3i_core(i, traini1, traini2, i1source, i2source, i1sourceA, i2sourceA, L, i2id, rank, topL, theta, maxscore);
					//use rank to get metrics values
					metrics_Bip3i(i, traini1, traini2, testi1, L, rank, &R, &PL);
				}
			}
			break;
		case 2:
			for (i = 0; i<traini1->maxId + 1; ++i) { //each user
				//if (i%1000 ==0) {printf("%d\n", i);fflush(stdout);}
				if (traini1->count[i]) {
					//get rank
					d_mass_Bip3i_core(i, traini1, traini2, i1source, i2source, i2sourceA, L, i2id, rank, topL, eta);
					//use rank to get metrics values
					metrics_Bip3i(i, traini1, traini2, testi1, L, rank, &R, &PL);
				}
			}
			break;
		case 3:
			for (i = 0; i<traini1->maxId + 1; ++i) { //each user
				//if (i%1000 ==0) {printf("%d\n", i);fflush(stdout);}
				if (traini1->count[i]) {
					//get rank
					thirdstepSD_mass_Bip3i_core(i, traini1, traini2, i1source, i2source, i2sourceA, L, i2id, rank, topL, epsilon, maxscore);
					//use rank to get metrics values
					metrics_Bip3i(i, traini1, traini2, testi1, L, rank, &R, &PL);
				}
			}
			break;
		case 4:
			for (i = 0; i<traini1->maxId + 1; ++i) { //each user
				//if (i%1000 ==0) {printf("%d\n", i);fflush(stdout);}
				if (traini1->count[i]) {
					//get rank
					hybrid_Bip3i_core(i, traini1, traini2, i1source, i2source, L, i2id, rank, topL, lambda);
					//use rank to get metrics values
					metrics_Bip3i(i, traini1, traini2, testi1, L, rank, &R, &PL);
				}
			}
			break;

	}
	R /= testi1->edgesNum;
	PL /= testi1->idNum;
	HL = metrics_HL_Bip3i(traini1, traini2, testi1, L, topL);
	IL = metrics_IL_Bip3i(traini1, traini2, testi1, L, topL, trainSim);
	NL = metrics_NL_Bip3i(traini1, traini2, testi1, L, topL);
	struct L_Bip3i *retn = create_L_Bip3i();
	retn->R = R;
	retn->PL = PL;
	retn->HL = HL;
	retn->IL = IL;
	retn->NL = NL;
	retn->topL = topL;
	retn->L = L;

	//printf("hybrid:\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", R, PL, IL, HL, NL);
	free(i1source);
	free(i2source);
	free(i1sourceA);
	free(i2sourceA);
	free(i2id);
	free(rank);
	return retn;
}

struct L_Bip3i *s_mass_Bip3i(struct Bip3i *traini1, struct Bip3i *traini2, struct Bip3i *testi1, struct Bip3i *testi2, struct iidNet *trainSim, double theta, int maxscore) {
	struct param_recommend_Bip3i param;
	param.theta = theta;
	param.maxscore = maxscore;
	return recommend_Bip3i(1, traini1, traini2, testi1, testi2, trainSim, param);
}

struct L_Bip3i *d_mass_Bip3i(struct Bip3i *traini1, struct Bip3i *traini2, struct Bip3i *testi1, struct Bip3i *testi2, struct iidNet *trainSim, double eta) {
	struct param_recommend_Bip3i param;
	param.eta = eta;
	return recommend_Bip3i(2, traini1, traini2, testi1, testi2, trainSim, param);
}

struct L_Bip3i *thirdstepSD_mass_Bip3i(struct Bip3i *traini1, struct Bip3i *traini2, struct Bip3i *testi1, struct Bip3i *testi2, struct iidNet *trainSim, double epsilon, int maxscore) {
	struct param_recommend_Bip3i param;
	param.epsilon = epsilon;
	param.maxscore = maxscore;
	return recommend_Bip3i(3, traini1, traini2, testi1, testi2, trainSim, param);
}

struct L_Bip3i *hybrid_Bip3i(struct Bip3i *traini1, struct Bip3i *traini2, struct Bip3i *testi1, struct Bip3i *testi2, struct iidNet *trainSim, double lambda) {
	struct param_recommend_Bip3i param;
	param.lambda = lambda;
	return recommend_Bip3i(3, traini1, traini2, testi1, testi2, trainSim, param);
}

struct iidLineFile *similarity_realtime_Bip3i(struct Bip3i *bipi1, struct Bip3i *bipi2) {
	printf("calculate similarity begin...\t");fflush(stdout);
	int i,j;
	int *sign = calloc((bipi1->maxId + 1),sizeof(int));
	assert(sign != NULL);

	struct iidLineFile *simfile = malloc(sizeof(struct iidLineFile));
	assert(simfile != NULL);

	int con = 1000000;
	struct iidLine *lines = malloc(con*sizeof(struct iidLine));
	assert(lines != NULL);
	long linesNum = 0;

	int i1Max=-1;
	int i1Min=INT_MAX;
	int i2Max=-1;
	int i2Min=INT_MAX;

	long k;
	int Sij;
	double soij;
	for (i=0; i<bipi2->maxId + 1; ++i) {
		if (bipi2->count[i]) {
			memset(sign, 0, (bipi1->maxId + 1)*sizeof(int));
			for (k=0; k<bipi2->count[i]; ++k) {
				sign[bipi2->id[i][k]] = 1;
			}
			for (j = i+1; j<bipi2->maxId + 1; ++j) {
				if (bipi2->count[j]) {
					Sij = 0;
					for (k=0; k<bipi2->count[j]; ++k) {
						if (sign[bipi2->id[j][k]]) {
							++Sij;
						}
					}
					if (Sij) {
						soij = (double)Sij/pow(bipi2->count[i] * bipi2->count[j], 0.5);
						//fprintf(fp, "%d, %d, %.17f\n", i, j, soij);
						i1Min = i1Min<i?i1Min:i;
						i1Max = i;
						i2Min = i2Min<j?i2Min:j;
						i2Max = i2Max>j?i2Max:j;
						lines[linesNum].i1 = i;
						lines[linesNum].i2 = j;
						lines[linesNum].d3 = soij;
						++linesNum;
						if (linesNum == con) {
							con += 1000000;
							struct iidLine *temp = realloc(lines, con*sizeof(struct iidLine));
							assert(temp != NULL);
							lines = temp;
						}
					}
				}
			}
		}
	}

	free(sign);

	simfile->i1Max = i1Max;
	simfile->i2Max = i2Max;
	simfile->i1Min = i1Min;
	simfile->i2Min = i2Min;
	simfile->linesNum = linesNum;
	simfile->lines = lines;
	printf("calculate similarity done.\n");fflush(stdout);
	return simfile;
}
