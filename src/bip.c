/**
 *
 */
#include "bip.h"
#include "base.h"
#include "mtprand.h"
#include "sort.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

//kind of simple, just create struct iiBip.
//index is 1, i1 of struct LineFile will be the index.
//index is 2, i2 of struct LineFile will be the index.
struct iiBip *create_iiBip(const struct LineFile * const lf, int index) {
	if (lf == NULL || lf->i1 == NULL || lf->i2 == NULL) isError("create_iiBip lf");
	if (index != 1 && index != 2) isError("create_iiBip index");

	//all elements of struct iiBip.
	int maxId;
	int minId;
	long countMax=-1;
	long countMin=LONG_MAX;
	long *count;
	int **edges;
	long edgesNum = lf->linesNum;

	int *i1 = lf->i1;
	int *i2 = lf->i2;


	long i;
	if (1 == index) {
		minId = maxId = i1[0];
		for(i=0; i<edgesNum; ++i) {
			maxId = maxId > i1[i] ? maxId : i1[i];
			minId = minId < i1[i] ? minId : i1[i];
		}
	}
	else {
		minId = maxId = i2[0];
		for(i=0; i<edgesNum; ++i) {
			maxId = maxId > i2[i] ? maxId : i2[i];
			minId = minId < i2[i] ? minId : i2[i];
		}
	}

	//once get maxId, the four points can be assigned with memory.
	count = calloc(maxId+1, sizeof(long));
	assert(count != NULL);
	edges = malloc((maxId+1)*sizeof(void *));
	assert(edges != NULL);

	//fill the count.
	if (1 == index) {
		for(i=0; i<edgesNum; ++i) {
			++count[i1[i]];
		}
	}
	else {
		for(i=0; i<edgesNum; ++i) {
			++count[i2[i]];
		}
	}

	//once get count, the three points which point to point can be assigned with memory.
	int j;
	int idNum=0;
	for(j=0; j<maxId+1; ++j) {
		if (count[j]>0) {
			countMax = countMax>count[j]?countMax:count[j];
			countMin = countMin<count[j]?countMin:count[j];
			++idNum;
			edges[j]=malloc(count[j]*sizeof(int));
			assert(edges[j]!=NULL);
		}
		else {
			edges[j] = NULL;
		}
	}

	//fill edges
	long *temp = calloc(maxId+1, sizeof(long));
	assert(temp!=NULL);
	if (1 == index) {
		for(i=0; i<edgesNum; ++i) {
			int ii1 =i1[i];
			edges[ii1][temp[ii1]]=i2[i];
			++temp[ii1];
		}
	}
	else {
		for(i=0; i<edgesNum; ++i) {
			int ii2 =i2[i];
			edges[ii2][temp[ii2]]=i1[i];
			++temp[ii2];
		}
	}
	free(temp);

	//fill Bip and return.
	struct iiBip *Bip = malloc(sizeof(struct iiBip));
	assert(Bip != NULL);

	Bip->maxId=maxId;
	Bip->minId=minId;
	Bip->idNum=idNum;
	Bip->countMax = countMax;
	Bip->countMin = countMin;
	Bip->count=count;
	Bip->edges= edges;
	Bip->edgesNum=edgesNum;

	if (index == 1) {
		printf("create i1 bip from %s =>> Max: %5d, Min: %5d, Num: %5d, countMax: %5ld, countMin: %5ld, edgesNum: %5ld\n", lf->filename, maxId, minId, idNum, countMax, countMin, edgesNum); fflush(stdout);
	}
	else {
		printf("create i2 bip from %s =>> Max: %5d, Min: %5d, Num: %5d, countMax: %5ld, countMin: %5ld, edgesNum: %5ld\n", lf->filename, maxId, minId, idNum, countMax, countMin, edgesNum); fflush(stdout);
	}

	return Bip;
}

void free_iiBip(struct iiBip *bip) {
	int i=0;
	for(i=0; i<bip->maxId+1; ++i) {
		free(bip->edges[i]);
	}
	free(bip->count);
	free(bip->edges);
	free(bip);
}

struct iiBip * clone_iiBip(struct iiBip *bip) {
	struct iiBip *new = malloc(sizeof(struct iiBip));
	new->count = malloc((bip->maxId + 1)*sizeof(long));
	new->edges = malloc((bip->maxId + 1)*sizeof(void *));
	memcpy(new->count, bip->count, (bip->maxId + 1)*sizeof(long));
	int i;
	for (i=0; i<bip->maxId +1 ; ++i) {
		if (new->count[i]) {
			new->edges[i] = malloc(new->count[i]*sizeof(int));
			memcpy(new->edges[i], bip->edges[i], new->count[i]*sizeof(int));
		}
		else {
			new->edges[i] = NULL;
		}
	}

	new->maxId = bip->maxId;
	new->minId = bip->minId;
	new->idNum = bip->idNum;
	new->countMax = bip->countMax;
	new->countMin = bip->countMin;
	new->edgesNum = bip->edgesNum;

	return new;
}

void *verify_iiBip(struct iiBip *bipi1, struct iiBip *bipi2) {
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
				int origin = bipi1->edges[j][i];
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
		printf("verifyiiBip: perfect network.\n");
	}
	return (void *)0;
}

//divide iiBip into two parts.
//return two struct LineFile. 
//the first one is always the small one.
//the second is always the large one.
static char *divfilename[2] = {"trainset", "testset "};
void divide_iiBip(struct iiBip *bipi1, struct iiBip *bipi2, double rate, struct LineFile **small, struct LineFile **big) {
	if (rate <=0 || rate >= 1) {
		isError("divide_iiBip error: wrong rate.\n");
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

	*small = init_LineFile();
	*big = init_LineFile();

	(*small)->i1 = malloc(l1*sizeof(int));
	(*small)->i2 = malloc(l1*sizeof(int));
	(*big)->i1 = malloc(l2*sizeof(int));
	(*big)->i2 = malloc(l2*sizeof(int));

	long line1=0, line2=0;

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

	int i, neigh;
	long j;
	for (i=0; i<bipi1->maxId + 1; ++i) {
		for (j=0; j<bipi1->count[i]; ++j) {
			neigh = bipi1->edges[i][j];
			if (get_d01_MTPR() < rate) {
				if ((counti1[i] == 1 && i1sign[i] == 0) || (counti2[neigh] == 1 && i2sign[neigh] == 0)) {
					(*big)->i1[line2] = i;	
					(*big)->i2[line2] = neigh;	
					--counti1[i];
					--counti2[neigh];
					i1sign[i] = 1;
					i2sign[neigh] = 1;
					++line2;
					continue;
				}
				(*small)->i1[line1] = i;	
				(*small)->i2[line1] = neigh;	
				--counti1[i];
				--counti2[neigh];
				++line1;
			}
			else {
				(*big)->i1[line2] = i;	
				(*big)->i2[line2] = neigh;	
				i1sign[i] = 1;
				i2sign[neigh] = 1;
				--counti1[i];
				--counti2[neigh];
				++line2;
			}
		}
	}
	if ((line1 > l1) && (line2 > l2)) {
		isError("divide_iiBip: l1 and l2 two small.\n");
	}

	free(i1sign);
	free(i2sign);
	free(counti1);
	free(counti2);

	(*big)->linesNum = line2;
	(*small)->linesNum = line1;

	(*big)->filename = divfilename[0];
	(*small)->filename = divfilename[1];

	printf("divide iiBip into train&test set =>> rate: %.3f, big file's linesNum: %ld, small file's linesNum: %ld.\n", rate, line2, line1);fflush(stdout);
}

//if target == 1, then calculate i1(mostly user)'s similarity.
//if target == 2, then calculate i2(mostly item)'s similarity.
static char *simfilename[3] = {"", "i1_similiarity", "i2_similiarity"};
struct LineFile *similarity_iiBip(struct iiBip *bipi1, struct iiBip *bipi2, int target) {
	if (target != 1 && target != 2) isError("similarity_iiBip target");
	int idmax, idmax2;
	long *count;
	int **edges;
	if (target == 1) {
		idmax = bipi1->maxId;
		idmax2 = bipi2->maxId;
		count = bipi1->count;
		edges = bipi1->edges;
	}
	else {
		idmax = bipi2->maxId;
		idmax2 = bipi1->maxId;
		count = bipi2->count;
		edges = bipi2->edges;
	}

	int *sign = calloc((idmax2 + 1),sizeof(int));
	assert(sign != NULL);

	struct LineFile *simfile = init_LineFile();
	assert(simfile != NULL);

	int con = 1000000;
	int *i1 = malloc(con*sizeof(int));
	int *i2 = malloc(con*sizeof(int));
	double *d1 = malloc(con*sizeof(double));

	long linesNum = 0;

	long k;
	int Sij;
	double soij;
	int i,j;
	for (i=0; i<idmax; ++i) {
		if (count[i]) {
			memset(sign, 0, (idmax2 + 1)*sizeof(int));
			for (k=0; k<count[i]; ++k) {
				sign[edges[i][k]] = 1;
			}
			for (j = i+1; j<idmax + 1; ++j) {
				if (count[j]) {
					Sij = 0;
					for (k=0; k<count[j]; ++k) {
						Sij += sign[edges[j][k]];
					}
					if (Sij) {
						soij = (double)Sij/pow(count[i] * count[j], 0.5);
						//fprintf(fp, "%d, %d, %.17f\n", i, j, soij);
						i1[linesNum] = i;
						i2[linesNum] = j;
						d1[linesNum] = soij;
						++linesNum;
						if (linesNum == con) {
							con += 1000000;
							int *temp = realloc(i1, con*sizeof(int));
							assert(temp != NULL);
							i1 = temp;
							temp = realloc(i2, con*sizeof(int));
							assert(temp != NULL);
							i2 = temp;
							double *tmp = realloc(d1, con*sizeof(double));
							assert(tmp != NULL);
							d1 = tmp;
						}
					}
				}
			}
		}
	}

	free(sign);

	simfile->linesNum = linesNum;
	simfile->i1 = i1;
	simfile->i2 = i2;
	simfile->d1 = d1;
	simfile->filename = simfilename[target];	

	printf("calculate %s done =>> linesNum: %ld.\n", simfile->filename, linesNum);
	return simfile;
}

struct LineFile *mass_similarity_iiBip(struct iiBip *bipi1, struct iiBip *bipi2) {


	struct LineFile *simfile = init_LineFile();

	int con = 1000000;
	int *i1 = malloc(con*sizeof(int));
	int *i2 = malloc(con*sizeof(int));
	double *d1 = malloc(con*sizeof(double));

	long linesNum = 0;

	double *i2source = malloc((bipi2->maxId + 1)*sizeof(double));
	double *i1source = malloc((bipi1->maxId + 1)*sizeof(double));

	int i, j, neigh;
	long degree;
	double source;

	int k;
	for (k=0; k<bipi1->maxId + 1; ++k) {
		if (bipi1->count[k]) {
			memset(i2source, 0, (bipi2->maxId+1)*sizeof(double));
			for (j=0; j<bipi1->count[k]; ++j) {
				neigh = bipi1->edges[k][j];
				i2source[neigh] = 1.0;
			}
			memset(i1source, 0, (bipi1->maxId+1)*sizeof(double));
			for (i=0; i<bipi2->maxId + 1; ++i) {
				if (i2source[i]) {
					degree = bipi2->count[i];
					source = i2source[i]/(double)degree;
					for (j=0; j<degree; ++j) {
						neigh = bipi2->edges[i][j];
						i1source[neigh] += source;
					}
				}
			}
			for (i=0; i<bipi1->maxId + 1; ++i) {
				if (i1source[i] && i!=k) {

					i1[linesNum] = k;
					i2[linesNum] = i;
					d1[linesNum] = i1source[i];
					++linesNum;

					if (linesNum == con) {
						con += 1000000;
						int *temp = realloc(i1, con*sizeof(int));
						assert(temp != NULL);
						i1 = temp;
						temp = realloc(i2, con*sizeof(int));
						assert(temp != NULL);
						i2 = temp;
						double *tmp = realloc(d1, con*sizeof(double));
						assert(tmp != NULL);
						d1 = tmp;
					}
				}
			}
		}
	}

	simfile->linesNum = linesNum;
	simfile->i1 = i1;
	simfile->i2 = i2;
	simfile->d1 = d1;
	printf("calculate mass similarity done.\n");
	free(i1source);
	free(i2source);
	return simfile;
}

struct Metrics_iiBip *create_MetricsiiBip(void) {
	struct Metrics_iiBip *lp = malloc(sizeof(struct Metrics_iiBip));
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
void clean_MetricsiiBip(struct Metrics_iiBip *lp) {
	lp->R = 0;
	lp->PL = 0;
	lp->HL = 0;
	lp->IL = 0;
	lp->NL = 0;
	lp->L = 0;
	free(lp->topL);
	lp->topL = NULL;
}

void free_MetricsiiBip(struct Metrics_iiBip *lp) {
	free(lp->topL);
	free(lp);
}

struct iiBip_recommend_param{
	int i1;

	int mass_topk;

	double HNBI_param;

	double RENBI_param;

	double hybrid_param;

	struct iidNet *userSim;
	struct iidNet *itemSim;

	struct iiBip *traini1;
	struct iiBip *traini2;
	struct iiBip *testi1;
	struct iiBip *testi2;

	double *i1source;
	double *i2source;
	int *i1id;
	int *i2id;
	double *i2sourceA;

};

//following is for recommendation.
//R is rankscore.
//PL is precision
//Warning: about unselected_list_length, I use bipii->idNum, not bipii->maxId. 
//	but I believe in linyuan's paper, she use the bipii->maxId. 
//	I think bipii->idNum make more sence.
static void metrics_R_PL_iiBip(int i1, long *i1count, int i2idNum, struct iiBip *testi1, int L, int *rank, double *R, double *PL) {
	if (i1<testi1->maxId + 1 &&  testi1->count[i1]) {
		int unselected_list_length = i2idNum - i1count[i1];
		int rank_i1_j = 0;
		int DiL = 0;
		int j, id;
		for (j=0; j<testi1->count[i1]; ++j) {
			id = testi1->edges[i1][j];
			rank_i1_j += rank[id];
			if (rank[id] <= L) {
				++DiL;
			}
		}
		*R += (double)rank_i1_j/(double)unselected_list_length;
		*PL += (double)DiL/(double)L;
	}
}
//IL is intrasimilarity
static double metrics_IL_iiBip(int i1maxId, long *i1count, int i1idNum, int i2maxId, int L, int *Hij, struct iidNet *sim) {
	if (!sim) return -1;
	double *sign = calloc((i2maxId + 1), sizeof(double));
	assert(sign != NULL);
	int i, j;
	long k;
	double IL = 0;
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1count[i]) {
			int *tmp = Hij + i*L;
			for (j=0; j<L; ++j) {
				int id = tmp[j];
				memset(sign, 0, (i2maxId + 1)*sizeof(double));
				for (k=0; k<sim->count[id]; ++k) {
					sign[sim->edges[id][k]] = sim->d[id][k];
				}
				for (k=j+1; k<L; ++k) {
					id = tmp[k];
					IL += sign[id];
				}
			}
		}
	}
	free(sign);
	IL /= L*(L-1)*i1idNum;
	return 2*IL;
}
//HL is hamming distance.
static double metrics_HL_iiBip(int i1maxId, long *i1count, int i2maxId, int L, int *Hij) {
	int *sign = calloc((i2maxId + 1), sizeof(int));
	assert(sign != NULL);
	int i, j;
	long k;
	int cou = 0;
	int Cij = 0;
	double HL = 0;
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1count[i]) {
			memset(sign, 0, (i2maxId + 1)*sizeof(int));
			for (k=i*L; k<i*L+L; ++k) {
				sign[Hij[k]] = 1;
			}
			for (j=i+1; j<i1maxId + 1; ++j) {
				if (i1count[j]) {
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
static double metrics_NL_iiBip(int i1maxId, long *i1count, int i1idNum, long *i2count, int L, int *Hij) {
	int i,j;
	long NL = 0;
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1count[i]) {
			int *tmp = Hij + i*L;
			for (j=0; j<L; ++j) {
				NL += i2count[tmp[j]];
			}
		}
	}
	NL /= L*i1idNum;
	return NL;
}

static inline void iiBip_core_common_part(struct iiBip_recommend_param *args, int *i2id, int *rank, int *topL_i1L, int L) {
	
	int uid = args->i1;
	long uidCount = args->traini1->count[uid];
	int *uidId = args->traini1->edges[uid];
	int i2maxId = args->traini2->maxId;
	long *i2count = args->traini2->count;
	double *i2source = args->i2source;

	long i;
	//set selected item's source to -1
	for (i=0; i<uidCount; ++i) {
		i2source[uidId[i]] = -1;
		//i2source[uidId[i]] = 0;
	}
	//set i2id and rank.
	int j=0;
	double dtmp;
	for (i=0; i<i2maxId + 1; ++i) {
		i2id[i] = i;
		//set unexisted item's source to -2.
		if (!i2count[i]) {
			i2source[i] = -2;
		}

		if (i2source[i] > 0 ) {
			dtmp = i2source[j];
			i2source[j] = i2source[i];
			i2source[i] = dtmp;

			i2id[i] = i2id[j];
			i2id[j] = i;

			++j;
			
		}
	}
	//to this step, i2source contains four parts: 
	//1, nomral i2source[x] , which <0, 1].
	//2, i2source[x] = 0, which x isn't selected by anyone has similarity.
	//3, i2source[x] = -1, which x has been selected by i1.
	//4, i2source[x] = -2, which x is the hole, x isn't selected by anyone.
	//
	//after qsort_di_desc, the id of the item with most source will be in i2id[0];
	//qsort_di_desc(i2source, 0, i2maxId, i2id);
	//printf("%d\t%d\n", j, i2maxId);
	qsort_di_desc(i2source, 0, j-1, i2id);
	//copy the top L itemid into topL.
	memcpy(topL_i1L, i2id, L*sizeof(int));
	//get rank;
	for (i=0; i<i2maxId + 1; ++i) {
		rank[i2id[i]] = i+1;
	}
}

//three-step random walk of Probs
static void mass_recommend_iiBip(struct iiBip_recommend_param *args) {

	int i1 = args->i1;
	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	int i, j, neigh;
	long degree;
	double source;
	//one 
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			degree = i1count[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
	}
}

//three-step random walk of Probs
static void mass_topk_recommend_iiBip(struct iiBip_recommend_param *args) {

	int i1 = args->i1;
    struct iidNet *userSim = args->userSim;
	int topR = args->mass_topk;

	double * i1source = args->i1source;
	double *i2source = args->i2source;

	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	int i, j, neigh;
	long degree;
	double source;
	//one 
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	long k;
	for (k=0; k<userSim->count[i1]; ++k) {
		i = userSim->edges[i1][k];
		if (k < topR) {
			degree = i1count[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
		else {
			break;
		}
	}
}

//three-step random walk of heats
static void heats_recommend_iiBip(struct iiBip_recommend_param *args) {
	int i1 = args->i1;
	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	int neigh, i;
	long j;
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1;
	}
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1count[i]) {
			for (j=0; j<i1count[i]; ++j) {
				neigh = i1ids[i][j];
				i1source[i] += i2source[neigh];
			}
			i1source[i] /= i1count[i];
		}
	}
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2count[i]) {
			for (j=0; j<i2count[i]; ++j) {
				neigh = i2ids[i][j];
				i2source[i] += i1source[neigh];
			}
			i2source[i] /= i2count[i];
		}
	}
}

//three-step random walk of HNBI
static void HNBI_recommend_iiBip(struct iiBip_recommend_param *args) {
	int i1 = args->i1;
	double HNBI_param = args->HNBI_param;

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	int i, j, neigh;
	long degree;
	double source;
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0*pow(i2count[neigh], HNBI_param);
	}
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			degree = i1count[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
	}
}

//five-step random walk of RENBI
static void RENBI_recommend_iiBip(struct iiBip_recommend_param *args) {
	int i1 = args->i1;
	double RENBI_param = args->RENBI_param;
	double *i2sourceA = args->i2sourceA;

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	int i, j, neigh;
	long degree;
	double source;
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	//one
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			degree = i1count[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
	}
	//save three steps result	
	memcpy(i2sourceA, i2source, (i2maxId + 1)*sizeof(double));
	//four
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//five
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			degree = i1count[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
	}
	//now, i2source save w*w result.
	//add: w+RENBI_param*w*w.
	for (i=0; i<i2maxId + 1; ++i) {
		i2sourceA[i] += RENBI_param*i2source[i];
	}

	memcpy(i2source, i2sourceA, (i2maxId + 1)*sizeof(double));
}

//three-step random walk of hybrid
static void hybrid_recommend_iiBip(struct iiBip_recommend_param *args) {
	int i1 = args->i1;
	double hybrid_param = args->hybrid_param;

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	long *i1count = args->traini1->count;
	long *i2count = args->traini2->count;

	int neigh, i;
	//double source;
	long j;
	//one
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			double powl = pow(i2count[i], hybrid_param);
			for (j=0; j<i2count[i]; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += i2source[i]/powl;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2count[i]) {
			double powl = pow(i2count[i], 1-hybrid_param);
			for (j=0; j<i2count[i]; ++j) {
				neigh = i2ids[i][j];
				i2source[i] += i1source[neigh]/i1count[neigh];
			}
			i2source[i] /= powl;
		}
	}
}


/** 
 * core function of recommendation.
 * type :
 * 1  -- mass (NONE arg)
 * 3  -- mass_corK (under development)
 * 4  -- heats (NONE arg)
 * 5  -- HNBI  (double HNBI_param)
 * 6  -- RENBI  (RENBI_param)
 * 7  -- hybrid (hybrid_param)
 *
 * all L is from this function. if you want to change, change the L below.
 */
static struct Metrics_iiBip *recommend_iiBip(void (*recommend_core)(struct iiBip_recommend_param *), struct iiBip_recommend_param *args) {

	int i1maxId      = args->traini1->maxId;
	int i2maxId      = args->traini2->maxId;
	int i1idNum      = args->traini1->idNum;
	int i2idNum      = args->traini2->idNum;
	long *i1count    = args->traini1->count;
	long *i2count    = args->traini2->count;

	struct iidNet *itemSim = args->itemSim;

 	// all L is from this function. if you want to change, change the L below.
	int L = 50;

	double R, PL, HL, IL, NL;
	R=PL=HL=IL=NL=0;

	double *i1source = malloc((i1maxId + 1)*sizeof(double));
	assert(i1source != NULL);
	double *i2source = malloc((i2maxId + 1)*sizeof(double));
	assert(i2source != NULL);
	args->i1source = i1source;
	args->i2source = i2source;

	double *i2sourceA = malloc((i2maxId + 1)*sizeof(double));
	assert(i2sourceA != NULL);
	args->i2sourceA = i2sourceA;

	int *rank = malloc((i2maxId + 1)*sizeof(int));
	assert(rank != NULL);
	int *i1id =  malloc((i1maxId + 1)*sizeof(int));
	assert(i1id != NULL);
	int *i2id =  malloc((i2maxId + 1)*sizeof(int));
	assert(i2id != NULL);
	args->i1id = i1id;
	args->i2id = i2id;

	int i;
	int *topL = calloc(L*(i1maxId + 1), sizeof(int));
	assert(topL != NULL);

	for (i = 0; i<i1maxId + 1; ++i) { //each user
		//if (i%1000 ==0) {printf("%d\n", i);fflush(stdout);}
		//only compute the i in both i1 and test.
		if (i1count[i]) {
			//get rank
			args->i1 = i;
			recommend_core(args);
			iiBip_core_common_part(args, i2id, rank, topL + i*L, L);
			metrics_R_PL_iiBip(i, i1count, /*i2maxId*/i2idNum, args->testi1, L, rank, &R, &PL);

		}
		//printf("%d\t", i);fflush(stdout);
	}

	R /= args->testi1->edgesNum;
	PL /= args->testi1->idNum;
	HL = metrics_HL_iiBip(i1maxId, i1count, i2maxId, L, topL);
	IL = metrics_IL_iiBip(i1maxId, i1count, i1idNum, i2maxId, L, topL, itemSim);
	NL = metrics_NL_iiBip(i1maxId, i1count, i1idNum, i2count, L, topL);
	
	struct Metrics_iiBip *retn = create_MetricsiiBip();
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
	free(i2sourceA);
	free(i1id);
	free(i2id);
	free(rank);
	return retn;
}


struct Metrics_iiBip *mass_iiBip(struct iiBip *traini1, struct iiBip *traini2, struct iiBip *testi1, struct iiBip *testi2, struct iidNet *itemSim) {
	struct iiBip_recommend_param param;
	param.itemSim = itemSim;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_iiBip(mass_recommend_iiBip, &param);
}

struct Metrics_iiBip *heats_iiBip(struct iiBip *traini1, struct iiBip *traini2, struct iiBip *testi1, struct iiBip *testi2, struct iidNet *itemSim) {
	struct iiBip_recommend_param param;
	param.itemSim = itemSim;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_iiBip(heats_recommend_iiBip, &param);
}

struct Metrics_iiBip *HNBI_iiBip(struct iiBip *traini1, struct iiBip *traini2, struct iiBip *testi1, struct iiBip *testi2, struct iidNet *itemSim, double HNBI_param) {
	struct iiBip_recommend_param param;
	param.itemSim = itemSim;
	param.HNBI_param = HNBI_param;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_iiBip(HNBI_recommend_iiBip, &param);
}

struct Metrics_iiBip *RENBI_iiBip(struct iiBip *traini1, struct iiBip *traini2, struct iiBip *testi1, struct iiBip *testi2, struct iidNet *itemSim, double RENBI_param) {
	struct iiBip_recommend_param param;
	param.itemSim = itemSim;
	param.RENBI_param = RENBI_param;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_iiBip(RENBI_recommend_iiBip, &param);
}

struct Metrics_iiBip *hybrid_iiBip(struct iiBip *traini1, struct iiBip *traini2, struct iiBip *testi1, struct iiBip *testi2, struct iidNet *itemSim, double hybrid_param) {
	struct iiBip_recommend_param param;
	param.itemSim = itemSim;
	param.hybrid_param = hybrid_param;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_iiBip(hybrid_recommend_iiBip, &param);
}

int *mass_getBK_iiBip(struct iiBip *traini1, struct iiBip *traini2, struct iiBip *testi1, struct iiBip *testi2, struct iidNet *userSim) {
	printf("calculate BK begin =>> ");
	print_time();

	double *i1source = malloc((traini1->maxId + 1)*sizeof(double));
	double *i2source = malloc((traini2->maxId + 1)*sizeof(double));
	int *rank = malloc((traini2->maxId + 1)*sizeof(int));
	int *i2id =  malloc((traini2->maxId + 1)*sizeof(int));

	double R, PL;

	struct iiBip_recommend_param args;
	args.traini1 = traini1;
	args.traini2 = traini2;
	args.testi1 = testi1;
	args.i1source = i1source;
	args.i2source = i2source;
	args.userSim = userSim;

	int i;
    int j;
	int L = 50;
	int *topL = calloc(L*(traini1->maxId + 1), sizeof(int));
	double bR, bPL;
	int bi;
	int *best = malloc((traini1->maxId + 1)*sizeof(int));
	double total_bR = 0, total_R = 0;
	double total_bPL = 0, total_PL = 0;

	for (i = 0; i<traini1->maxId + 1; ++i) { //each user
		if (i<testi1->maxId + 1 && testi1->count[i]) {
			//just to make sure bestR is enough big.
			bR = 10000; //big enough.
			bPL = -1;
			bi = -1;
			//to make realR2 is same with mass.
			for (j=1; j<= userSim->count[i] || (j ==1 && userSim->count[i] == 0); ++j) {

				//probs_knn_Bip_core(i, &args, userSim, j);
				args.i1 = i;
				args.mass_topk = j;
				mass_topk_recommend_iiBip(&args);

				
				iiBip_core_common_part(&args, i2id, rank, topL + i*L, L);

				R=PL=0;
				metrics_R_PL_iiBip(i, traini1->count, traini2->idNum, testi1, L, rank, &R, &PL);
				//R will never be 0, because i is in testi1.
				if (bR > R) {
					bR = R;
					bi = j;
				}
				if (bPL < PL) {
					bPL = PL;
				}
			}
			fclose(fp);
			
			total_bR += bR;
			total_R += R;
			total_bPL += bPL;
			total_PL += PL;

			best[i] = bi;

		}
		else {
			best[i] = -1;
		}
	}
	printf("BR result =>> Best R: %f, normal R: %f, Best PL: %f, normal PL: %f\n", total_bR/testi1->edgesNum, total_R/testi1->edgesNum, total_bPL/testi1->edgesNum, total_PL/testi1->edgesNum);

	free(i1source);
	free(i2source);
	free(rank);
	free(i2id);
	free(topL);
	printf("calculate BK done =>> \n");
	print_time();
	return best;
}
