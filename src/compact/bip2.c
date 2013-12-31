/**
 * struct L_Bip contains the result fo all kinds of recommendation algorithm.
 *
 */
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


/************************************************************************************************************/
/************************* biparte network recommendation core functions. ***********************************/
/*** they can be used by bip2 or bip3. **********************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

struct Bip_core_param {
	double theta;
	double eta;
	double lambda;
	double *score;
	double epsilon;
	double orate;
	int topR;
	int *knn;
	struct iidNet *userSim;
	struct iidNet *itemSim;
};

struct Bip_core_base {
	double *i1source;
	double *i2source;
	int **i1ids;
	int **i2ids;
	int i1maxId;
	int i2maxId;
	long *i1count;
	long *i2count;
	int i1idNum;
	int i2idNum;
};

struct Bip_core_test {
	int **id;
	int maxId;
	long *count;
	int idNum;
	long edgesNum;
};

struct L_Bip *create_L_Bip(void) {
	struct L_Bip *lp = malloc(sizeof(struct L_Bip));
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
void clean_L_Bip(struct L_Bip *lp) {
	lp->R = 0;
	lp->PL = 0;
	lp->HL = 0;
	lp->IL = 0;
	lp->NL = 0;
	lp->L = 0;
	free(lp->topL);
	lp->topL = NULL;
}
void free_L_Bip(struct L_Bip *lp) {
	free(lp->topL);
	free(lp);
}


//following is for recommendation.
//Warning: remeber the maxId in testset maybe smaller than the maxId in trainset.
//R is rankscore.
//PL is precision
//Warning: about unselected_list_length, I use bipi2->idNum, not bipi2->maxId. 
//	but I believe in linyuan's paper, she use the bipi2->maxId. 
//	I think bipi2->idNum make more sence.
static void metrics_Bip(int i1, long *i1count, int i2idNum, struct Bip_core_test *testi1, int L, int *rank, double *R, double *PL) {
	if (i1<testi1->maxId + 1 &&  testi1->count[i1]) {
		int unselected_list_length = i2idNum - i1count[i1];
		int rank_i1_j = 0;
		int DiL = 0;
		int j, id;
		for (j=0; j<testi1->count[i1]; ++j) {
			id = testi1->id[i1][j];
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
static double metrics_IL_Bip(int i1maxId, long *i1count, int i1idNum, int i2maxId, int L, int *Hij, struct iidNet *sim) {
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
	IL /= L*(L-1)*i1idNum;
	return 2*IL;
}
//HL is hamming distance.
static double metrics_HL_Bip(int i1maxId, long *i1count, int i2maxId, int L, int *Hij) {
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
static double metrics_NL_Bip(int i1maxId, long *i1count, int i1idNum, long *i2count, int L, int *Hij) {
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

static inline void Bip_core_common_part(int uid, struct Bip_core_base *args, int *i2id, int *rank, int *topL_i1L, int L) {
	long uidCount = args->i1count[uid];
	int *uidId = args->i1ids[uid];
	int i2maxId = args->i2maxId;
	long *i2count = args->i2count;
	double *i2source = args->i2source;

	long i;
	//set selected item's source to -1
	for (i=0; i<uidCount; ++i) {
		i2source[uidId[i]] = -1;
	}
	//set i2id and rank.
	for (i=0; i<i2maxId + 1; ++i) {
		rank[i] = i + 1;
		i2id[i] = i;
		//set unexisted item's source to -2.
		if (!i2count[i]) {
			i2source[i] = -2;
		}
	}
	//to this step, i2source contains four parts: 
	//1, nomral i2source[x] , which <0, 1].
	//2, i2source[x] = 0, which x isn't selected by anyone has similarity.
	//3, i2source[x] = -1, which x has been selected by i1.
	//4, i2source[x] = -2, which x is the hole, x isn't selected by anyone.
	//
	//after qsort_di_desc, the id of the item with most source will be in i2id[0];
	qsort_di_desc(i2source, 0, i2maxId, i2id);
	//copy the top L itemid into topL.
	memcpy(topL_i1L, i2id, L*sizeof(int));
	//get rank;
	for (i=0; i<i2maxId + 1; ++i) {
		rank[i2id[i]] = i+1;
	}
}

//three-step random walk of Probs
static void probs_Bip_core(int i1, struct Bip_core_base *args) {
	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->i1ids;
	int **i2ids = args->i2ids; 
	int i1maxId = args->i1maxId;
	int i2maxId = args->i2maxId;
	long *i1count = args->i1count;
	long *i2count = args->i2count;

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

//three-step random walk of heats
static void heats_Bip_core(int i1, struct Bip_core_base *args) {

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->i1ids;
	int **i2ids = args->i2ids; 
	int i1maxId = args->i1maxId;
	int i2maxId = args->i2maxId;
	long *i1count = args->i1count;
	long *i2count = args->i2count;
	
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
static void HNBI_Bip_core(int i1, struct Bip_core_base *args, double theta) {
	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->i1ids;
	int **i2ids = args->i2ids; 
	int i1maxId = args->i1maxId;
	int i2maxId = args->i2maxId;
	long *i1count = args->i1count;
	long *i2count = args->i2count;
	int i, j, neigh;
	long degree;
	double source;
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0*pow(i2count[neigh], theta);
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
static void RENBI_Bip_core(int i1, struct Bip_core_base *args, double *i2sourceA, double eta) {

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->i1ids;
	int **i2ids = args->i2ids; 
	int i1maxId = args->i1maxId;
	int i2maxId = args->i2maxId;
	long *i1count = args->i1count;
	long *i2count = args->i2count;

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
	//add: w+eta*w*w.
	for (i=0; i<i2maxId + 1; ++i) {
		i2sourceA[i] += eta*i2source[i];
	}

	memcpy(i2source, i2sourceA, (i2maxId + 1)*sizeof(double));
}
//three-step random walk of hybrid
static void hybrid_Bip_core(int i1, struct Bip_core_base *args, double lambda) {

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->i1ids;
	int **i2ids = args->i2ids; 
	int i1maxId = args->i1maxId;
	int i2maxId = args->i2maxId;
	long *i1count = args->i1count;
	long *i2count = args->i2count;

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
			double powl = pow(i2count[i], lambda);
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
			double powl = pow(i2count[i], 1-lambda);
			for (j=0; j<i2count[i]; ++j) {
				neigh = i2ids[i][j];
				i2source[i] += i1source[neigh]/i1count[neigh];
			}
			i2source[i] /= powl;
		}
	}
}
//three-step random walk of Probs
static void onion_probs_Bip_core(int i1, struct Bip_core_base *args, struct iidNet *userSim, double orate) {
	if (orate > 1 || orate < 0) {
		isError("onion_probs_Bip_core wrong orate");
	}

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->i1ids;
	int **i2ids = args->i2ids; 
	int i1maxId = args->i1maxId;
	int i2maxId = args->i2maxId;
	long *i1count = args->i1count;
	long *i2count = args->i2count;

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
	double sim;
	long k;
	for (k=0; k<userSim->count[i1]; ++k) {
		i = userSim->edges[i1][k];
		sim = userSim->d3[i1][k];
		if (k == 0 || sim > orate) {
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
static void topR_probs_Bip_core(int i1, struct Bip_core_base *args, struct iidNet *userSim, int topR) {

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->i1ids;
	int **i2ids = args->i2ids; 
	int i1maxId = args->i1maxId;
	int i2maxId = args->i2maxId;
	long *i1count = args->i1count;
	long *i2count = args->i2count;

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
		if (k == 0 || k < topR) {
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
static void probs_knn_Bip_core(int i1, struct Bip_core_base *args, struct iidNet *userSim, int bestR) {

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->i1ids;
	int **i2ids = args->i2ids; 
	int i1maxId = args->i1maxId;
	int i2maxId = args->i2maxId;
	long *i1count = args->i1count;
	long *i2count = args->i2count;

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
	for (k=0; k<bestR; ++k) {
		i = userSim->edges[i1][k];
		degree = i1count[i];
		source = (double)i1source[i]/(double)degree;
		for (j=0; j<degree; ++j) {
			neigh = i1ids[i][j];
			i2source[neigh] += source;
		}
	}
}


/** 
 * core function of recommendation.
 * type :
 * 0 -- for test TODO deleted
 * 1 -- probs (NONE arg)
 * 2 -- heats (NONE arg)
 * 3 -- HNBI  (theta)
 * 4 -- RENBI  (eta)
 * 5 -- hybrid (lambda)
 * 6 -- score hybrid (epsilon) TODO deleted
 * 7 -- usersim onion probs (orate, userSim)
 * 8 -- usersim degree probs (orate, userSim)
 * 9 -- knn 
 *
 * all L is from this function. if you want to change, change the L below.
 */
static struct L_Bip *recommend_Bip(int type, struct Bip_core_base *args, struct Bip_core_param *param, struct Bip_core_test *test) {
	double theta  = param->theta;
	double eta    = param->eta;
	double lambda = param->lambda;
	double orate  = param->orate;
	int topR      = param->topR;
	int *knn      = param->knn;
	struct iidNet *userSim = param->userSim;
	struct iidNet *itemSim = param->itemSim;

	//int **i1ids      = args->i1ids;
	//int **i2ids      = args->i2ids; 
	int i1maxId      = args->i1maxId;
	int i2maxId      = args->i2maxId;
	int i1idNum      = args->i1idNum;
	int i2idNum      = args->i2idNum;
	long *i1count    = args->i1count;
	long *i2count    = args->i2count;

	int L = 50;

	double R, PL, HL, IL, NL;
	R=PL=HL=IL=NL=0;

	double *i1source = malloc((i1maxId + 1)*sizeof(double));
	assert(i1source != NULL);
	double *i2source = malloc((i2maxId + 1)*sizeof(double));
	assert(i2source != NULL);
	args->i1source = i1source;
	args->i2source = i2source;

	int *rank = malloc((i2maxId + 1)*sizeof(int));
	assert(rank != NULL);
	int *i2id =  malloc((i2maxId + 1)*sizeof(int));
	assert(i2id != NULL);

	int i1, i;
	int *topL = calloc(L*(i1maxId + 1), sizeof(int));
	assert(topL != NULL);
	switch (type) {
		case 1:
			for (i = 0; i<i1maxId + 1; ++i) { //each user
				if (i1count[i]) {
					//get rank
					probs_Bip_core(i, args);
					Bip_core_common_part(i, args, i2id, rank, topL + i*L, L);
					//use rank to get metrics values
					metrics_Bip(i, i1count, i2idNum, test, L, rank, &R, &PL);
				}
			}
			break;
		case 2:
			for (i1 = 0; i1<i1maxId + 1; ++i1) { //each user
				if (i1count[i1]) {
					heats_Bip_core(i1, args);
					Bip_core_common_part(i1, args, i2id, rank, topL + i1*L, L);
					metrics_Bip(i1, i1count, i2idNum, test, L, rank, &R, &PL);
				}
			}
			break;
		case 3:
			for (i1 = 0; i1<i1maxId + 1; ++i1) { //each user
				//if (i1%1000 ==0) {printf("%d\n", i1);fflush(stdout);}
				if (i1count[i1]) {
					HNBI_Bip_core(i1, args, theta);
					Bip_core_common_part(i1, args, i2id, rank, topL + i1*L, L);
					metrics_Bip(i1, i1count, i2idNum, test, L, rank, &R, &PL);
				}
			}
			break;
		case 4:
			assert(i2source != NULL);
			double *i2sourceA = calloc((i2maxId + 1),sizeof(double));
			for (i = 0; i<i1maxId + 1; ++i) { //each user
				//if (i%1000 ==0) {printf("%d\n", i);fflush(stdout);}
				//only compute the i in both i1 and test.
				if (i1count[i]) {
					//get rank
					RENBI_Bip_core(i, args, i2sourceA, eta);
					Bip_core_common_part(i, args, i2id, rank, topL + i*L, L);
					//use rank to get metrics values
					metrics_Bip(i, i1count, i2idNum, test, L, rank, &R, &PL);
				}
			}
			free(i2sourceA);
			break;
		case 5:
			for (i1 = 0; i1<i1maxId + 1; ++i1) { //each user
				//if (i1%1000 ==0) {printf("%d\n", i1);fflush(stdout);}
				if (i1count[i1]) {
					hybrid_Bip_core(i1, args, lambda);
					Bip_core_common_part(i1, args, i2id, rank, topL + i1*L, L);
					metrics_Bip(i1, i1count, i2idNum, test, L, rank, &R, &PL);
				}
			}
			break;
		case 7:
			for (i = 0; i<i1maxId + 1; ++i) { //each user
				//if (i%1000 ==0) {printf("%d\n", i);fflush(stdout);}
				//only compute the i in both i1 and test.
				if (i1count[i]) {
					//get rank
					onion_probs_Bip_core(i, args, userSim, orate);
					Bip_core_common_part(i, args, i2id, rank, topL + i*L, L);
					//use rank to get metrics values
					metrics_Bip(i, i1count, i2idNum, test, L, rank, &R, &PL);
				}
			}
			break;
		case 8:
			for (i = 0; i<i1maxId + 1; ++i) { //each user
				//if (i%1000 ==0) {printf("%d\n", i);fflush(stdout);}
				//only compute the i in both i1 and test.
				if (i1count[i]) {
					//get rank
					topR_probs_Bip_core(i, args, userSim, topR);
					Bip_core_common_part(i, args, i2id, rank, topL + i*L, L);
					//use rank to get metrics values
					metrics_Bip(i, i1count, i2idNum, test, L, rank, &R, &PL);
				}
			}
			break;
		case 9:
			for (i = 0; i<i1maxId + 1; ++i) { //each user
				//if (i%1000 ==0) {printf("%d\n", i);fflush(stdout);}
				//only compute the i in both i1 and test.
				if (i1count[i]) {
					//get rank
					probs_knn_Bip_core(i, args, userSim, knn[i]);
					Bip_core_common_part(i, args, i2id, rank, topL + i*L, L);
					//use rank to get metrics values
					metrics_Bip(i, i1count, i2idNum, test, L, rank, &R, &PL);
				}
			}
			break;
	}
	R /= test->edgesNum;
	PL /= test->idNum;
	HL = metrics_HL_Bip(i1maxId, i1count, i2maxId, L, topL);
	IL = metrics_IL_Bip(i1maxId, i1count, i1idNum, i2maxId, L, topL, itemSim);
	NL = metrics_NL_Bip(i1maxId, i1count, i1idNum, i2count, L, topL);
	struct L_Bip *retn = create_L_Bip();
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
	free(i2id);
	free(rank);
	return retn;
}

/************************************************************************************************************/
/************************* the following functions are related with BIP2. ***********************************/
/** include free, create, divide, verify, similarity. *******************************************************/
/** include 8 recommend algorithm. **************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

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

//kind of simple, just create struct Bip2.
//i1toi2 is 1, i1 of struct iiLineFile will be the index.
//i1toi2 is 0, i2 of struct iiLineFile will be the index.
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

//divide Bip2 into two parts.
//return two struct iiLineFile. the first one is always the small one.
//the second is always the large one.
struct iiLineFile *divide_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, double rate) {
	if (rate <=0 || rate >= 1) {
		printf("divide_Bip2 error: wrong rate.\n");
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
			neigh = bipi1->id[i][j];
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
					continue;
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
	printf("divide_Bip2 done:\n\trate: %f\n\tfile1: linesNum: %ld, i1Max: %d, i1Min: %d, i2Max: %d, i2Min: %d\n\tfile2: linesNum: %ld, i1Max: %d, i1Min: %d, i2Max: %d, i2Min: %d\n", rate, line1, i1Max, i1Min, i2Max, i2Min, line2, _i1Max, _i1Min, _i2Max, _i2Min);fflush(stdout);
	return twofile;
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

//if i1ori2 == 1, then calculate i1(user)'s similarity.
//if i1ori2 == 0, then calculate i2(item)'s similarity.
struct iidLineFile *similarity_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, int i1ori2) {
	int idmax, idmax2;
	long *count;
	int **id;
	if (i1ori2 == 1) {
		idmax = bipi1->maxId;
		idmax2 = bipi2->maxId;
		count = bipi1->count;
		id = bipi1->id;
	}
	else if (i1ori2 == 0) {
		idmax = bipi2->maxId;
		idmax2 = bipi1->maxId;
		count = bipi2->count;
		id = bipi2->id;
	}
	else {
		isError("similarity_Bip2");
	}
	int *sign = calloc((idmax2 + 1),sizeof(int));
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
	int i,j;
	for (i=0; i<idmax; ++i) {
		if (count[i]) {
			memset(sign, 0, (idmax2 + 1)*sizeof(int));
			for (k=0; k<count[i]; ++k) {
				sign[id[i][k]] = 1;
			}
			for (j = i+1; j<idmax + 1; ++j) {
				if (count[j]) {
					Sij = 0;
					for (k=0; k<count[j]; ++k) {
						Sij += sign[id[j][k]];
					}
					if (Sij) {
						soij = (double)Sij/pow(count[i] * count[j], 0.5);
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
	printf("calculate similarity done.\n");
	return simfile;
}

struct L_Bip *probs_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *itemSim) {
	struct Bip_core_param param;
	param.itemSim = itemSim;
	struct Bip_core_base args;
	args.i1maxId = bipi1->maxId;
	args.i2maxId = bipi2->maxId;
	args.i1count = bipi1->count;
	args.i2count = bipi2->count;
	args.i1idNum = bipi1->idNum;
	args.i2idNum = bipi2->idNum;
	args.i1ids = bipi1->id;
	args.i2ids = bipi2->id;
	struct Bip_core_test test;
	test.id = testi1->id;
	test.maxId = testi1->maxId;
	test.count = testi1->count;
	test.idNum = testi1->idNum;
	test.edgesNum = testi1->edgesNum;
	return recommend_Bip(1, &args, &param, &test);
}

struct L_Bip *heats_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *itemSim) {
	struct Bip_core_param param;
	param.itemSim = itemSim;
	struct Bip_core_base args;
	args.i1maxId = bipi1->maxId;
	args.i2maxId = bipi2->maxId;
	args.i1count = bipi1->count;
	args.i2count = bipi2->count;
	args.i1idNum = bipi1->idNum;
	args.i2idNum = bipi2->idNum;
	args.i1ids = bipi1->id;
	args.i2ids = bipi2->id;
	struct Bip_core_test test;
	test.id = testi1->id;
	test.maxId = testi1->maxId;
	test.count = testi1->count;
	test.idNum = testi1->idNum;
	test.edgesNum = testi1->edgesNum;
	return recommend_Bip(2, &args, &param, &test);
}

struct L_Bip *HNBI_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *itemSim, double theta) {
	struct Bip_core_param param;
	param.theta = theta;
	param.itemSim = itemSim;
	struct Bip_core_base args;
	args.i1maxId = bipi1->maxId;
	args.i2maxId = bipi2->maxId;
	args.i1count = bipi1->count;
	args.i2count = bipi2->count;
	args.i1idNum = bipi1->idNum;
	args.i2idNum = bipi2->idNum;
	args.i1ids = bipi1->id;
	args.i2ids = bipi2->id;
	struct Bip_core_test test;
	test.id = testi1->id;
	test.maxId = testi1->maxId;
	test.count = testi1->count;
	test.idNum = testi1->idNum;
	test.edgesNum = testi1->edgesNum;
	return recommend_Bip(3, &args, &param, &test);
}

struct L_Bip *RENBI_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *itemSim, double eta) {
	struct Bip_core_param param;
	param.eta = eta;
	param.itemSim = itemSim;
	struct Bip_core_base args;
	args.i1maxId = bipi1->maxId;
	args.i2maxId = bipi2->maxId;
	args.i1count = bipi1->count;
	args.i2count = bipi2->count;
	args.i1idNum = bipi1->idNum;
	args.i2idNum = bipi2->idNum;
	args.i1ids = bipi1->id;
	args.i2ids = bipi2->id;
	struct Bip_core_test test;
	test.id = testi1->id;
	test.maxId = testi1->maxId;
	test.count = testi1->count;
	test.idNum = testi1->idNum;
	test.edgesNum = testi1->edgesNum;
	return recommend_Bip(4, &args, &param, &test);
}

struct L_Bip *hybrid_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *itemSim, double lambda) {
	struct Bip_core_param param;
	param.lambda = lambda;
	param.itemSim = itemSim;
	struct Bip_core_base args;
	args.i1maxId = bipi1->maxId;
	args.i2maxId = bipi2->maxId;
	args.i1count = bipi1->count;
	args.i2count = bipi2->count;
	args.i1idNum = bipi1->idNum;
	args.i2idNum = bipi2->idNum;
	args.i1ids = bipi1->id;
	args.i2ids = bipi2->id;
	struct Bip_core_test test;
	test.id = testi1->id;
	test.maxId = testi1->maxId;
	test.count = testi1->count;
	test.idNum = testi1->idNum;
	test.edgesNum = testi1->edgesNum;
	return recommend_Bip(5, &args, &param, &test);
}

struct L_Bip *onion_mass_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *itemSim, struct iidNet *userSim, double orate) {
	struct Bip_core_param param;
	param.userSim = userSim;
	param.orate = orate;
	param.itemSim = itemSim;
	struct Bip_core_base args;
	args.i1maxId = bipi1->maxId;
	args.i2maxId = bipi2->maxId;
	args.i1count = bipi1->count;
	args.i2count = bipi2->count;
	args.i1idNum = bipi1->idNum;
	args.i2idNum = bipi2->idNum;
	args.i1ids = bipi1->id;
	args.i2ids = bipi2->id;
	struct Bip_core_test test;
	test.id = testi1->id;
	test.maxId = testi1->maxId;
	test.count = testi1->count;
	test.idNum = testi1->idNum;
	test.edgesNum = testi1->edgesNum;
	return recommend_Bip(7, &args, &param, &test);
}

struct L_Bip *topR_probs_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *itemSim, struct iidNet *userSim, int topR) {
	struct Bip_core_param param;
	param.userSim = userSim;
	param.topR = topR;
	param.itemSim = itemSim;
	struct Bip_core_base args;
	args.i1maxId = bipi1->maxId;
	args.i2maxId = bipi2->maxId;
	args.i1count = bipi1->count;
	args.i2count = bipi2->count;
	args.i1idNum = bipi1->idNum;
	args.i2idNum = bipi2->idNum;
	args.i1ids = bipi1->id;
	args.i2ids = bipi2->id;
	struct Bip_core_test test;
	test.id = testi1->id;
	test.maxId = testi1->maxId;
	test.count = testi1->count;
	test.idNum = testi1->idNum;
	test.edgesNum = testi1->edgesNum;
	return recommend_Bip(8, &args, &param, &test);
}

struct L_Bip *probs_knn_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *itemSim, struct iidNet *userSim, int *knn) {
	struct Bip_core_param param;
	param.knn = knn;
	param.userSim = userSim;
	param.itemSim = itemSim;
	struct Bip_core_base args;
	args.i1maxId = bipi1->maxId;
	args.i2maxId = bipi2->maxId;
	args.i1count = bipi1->count;
	args.i2count = bipi2->count;
	args.i1idNum = bipi1->idNum;
	args.i2idNum = bipi2->idNum;
	args.i1ids = bipi1->id;
	args.i2ids = bipi2->id;
	struct Bip_core_test test;
	test.id = testi1->id;
	test.maxId = testi1->maxId;
	test.count = testi1->count;
	test.idNum = testi1->idNum;
	test.edgesNum = testi1->edgesNum;
	return recommend_Bip(9, &args, &param, &test);
}

void knn_getbest_Bip2(struct Bip2 *traini1, struct Bip2 *traini2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *userSim, int *bestK_R, int *bestK_PL) {
	printf("\nbegin to calculat best knn....\n");fflush(stdout);
	double *i1source = malloc((traini1->maxId + 1)*sizeof(double));
	assert(i1source != NULL);
	double *i2source = malloc((traini2->maxId + 1)*sizeof(double));
	assert(i2source != NULL);
	int *rank = malloc((traini2->maxId + 1)*sizeof(int));
	assert(rank != NULL);
	int *i2id =  malloc((traini2->maxId + 1)*sizeof(int));
	assert(i2id != NULL);

	double R, PL;

	struct Bip_core_base args;
	args.i1source = i1source;
	args.i2source = i2source;
	args.i1maxId = traini1->maxId;
	args.i2maxId = traini2->maxId;
	args.i1ids = traini1->id;
	args.i2ids = traini2->id;
	args.i1idNum = traini1->idNum;
	args.i2idNum = traini2->idNum;
	args.i1count = traini1->count;
	args.i2count = traini2->count;


	struct Bip_core_test test;
	test.id = testi1->id;
	test.maxId = testi1->maxId;
	test.count = testi1->count;
	test.idNum = testi1->idNum;
	test.edgesNum = testi1->edgesNum;

	int i;
    int j;
	int L = 50;
	double bestR, bestPL;
	int bestRK, bestPLK;
	double realR = 0;
	for (i = 0; i<traini1->maxId + 1; ++i) { //each user
	//for (i = 0; i<10; ++i) { //each user
		//only compute user in testset.
		if (i<testi1->maxId + 1 && testi1->count[i]) {
			//just to make sure bestR is enough big.
			bestR = LONG_MAX;
			bestPL = -1;
			bestRK = bestPLK = -1;
			for (j=0; j<userSim->count[i]; ++j) {

				probs_knn_Bip_core(i, &args, userSim, j);

				long uidCount = args.i1count[i];
				int *uidId = args.i1ids[i];
				int i2maxId = args.i2maxId;
				long *i2count = args.i2count;

				long ii;
				for (ii=0; ii<uidCount; ++ii) {
					i2source[uidId[ii]] = -1;
				}
				for (ii=0; ii<i2maxId + 1; ++ii) {
					rank[ii] = ii + 1;
					i2id[ii] = ii;
					if (!i2count[ii]) {
						i2source[ii] = -2;
					}
				}
				qsort_di_desc(i2source, 0, i2maxId, i2id);
				for (ii=0; ii<i2maxId + 1; ++ii) {
					rank[i2id[ii]] = ii+1;
				}

				R=PL=0;
				metrics_Bip(i, traini1->count, traini2->idNum, &test, L, rank, &R, &PL);
				//printf("%d, %d, %f\n", i, j, R);
				//R will never be 0, because i is in testi1.
				if (bestR > R) {
					bestR = R;
					bestRK = j;
				}
				if (bestPL < PL) {
					bestPL = PL;
					bestPLK = j;
				}
			}
			bestK_R[i] = bestRK;
			bestK_PL[i] = bestPLK;
			realR += bestR;
			//only print useful bestK_R
			printf("%d, %d, %ld, %f, %f\n", i, bestK_R[i], userSim->count[i], bestK_R[i]/(double)userSim->count[i], bestR);fflush(stdout);
		}
		else {
			//this doesn't affect RankScore and Precision, but it does affect the other metrics.
			bestK_PL[i] = bestK_R[i] = userSim->count[i];
		}
	}
	//printf("%f\n", realR/testi1->edgesNum);fflush(stdout);

	free(i1source);
	free(i2source);
	free(rank);
	free(i2id);
	printf("calculat best knn done.\n\n");fflush(stdout);
}

/************************************************************************************************************/
/************************************************************************************************************/
/****** the following functions are not used by me any more, but they works fine, so I will keep them. ******/
/************************************************************************************************************/
/************************************************************************************************************/

//Bip2 contains eight members. this function will correct six wrong members according to two right ones.
//renew maxId/minId/countMax/countMin/idNum/edgesNum from count 
//!!!count and id is always good&right.
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
