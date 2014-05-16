#include "base.h"
#include "dataset.h"
#include "mtprand.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>

static char *CICLENETC[2] = {"CYCLE", "NON_CYCLE"};
static char *DIRECTNETC[2] = {"DIRECT", "NON_DIRECT"};

/**
 * for 2d lattices, L is the size of Height and Width of the lattice, L must > 1.
 * L = 2 is a special case. total linesNum:
 * 		cycle,		direct: 8
 * 		cycle,	non_direct: 4
 * 	non_cycle,		direct: 8
 * 	non_cycle, 	non_direct: 4
 * 	
 * when L > 2, total linesNum:
 * 		cycle, 		direct: L*L*4
 * 		cycle, 	non_direct: L*L*2
 * 	non_cycle, 		direct: L*(L-1)*4
 * 	non_cycle,	non_direct: L*(L-1)*2
 */
struct LineFile * lattice2d_DS(int L, enum CICLENET cc, enum DIRECTNET dd) {
	print2l("%s =>> begin......\n", __func__);
	if (L<2) isError("%s =>> L too small.\n", __func__);

	struct LineFile *file = create_LineFile(NULL);
	long linesNum = (L-1)*L*2;
	if (cc == CYCLE && L != 2) {
		linesNum += 2*L;
	}
	if (dd == DIRECT) {
		linesNum *= 2;
	}
	(void)CICLENETC; (void)DIRECTNETC; //get rid of unused warning when VERBOSE_LEVEL is smaller than 3.
	print3l("%s =>> Generate 2D Lattice, %s, %s, L: %d, linesNum: %ld\n", __func__, CICLENETC[cc], DIRECTNETC[dd], L, linesNum);

	int *i1 = malloc(linesNum * sizeof(int));
	int *i2 = malloc(linesNum * sizeof(int));

	int i,j;
	long k=0;
	for (i=1; i<L; ++i) {
		for (j=1; j<L; ++j) {
			int id = i*L + j;
			i1[k]=id;
			i2[k]=id-1;	
			++k;
			if (dd == DIRECT) {
				i2[k]=id;
				i1[k]=id-1;	
				++k;
			}
			i1[k]=id;
			i2[k]=id-L;
			++k;
			if (dd == DIRECT) {
				i2[k]=id;
				i1[k]=id-L;
				++k;
			}
		}
	}
	for (i=1; i<L; ++i) {
		i1[k] = i;
		i2[k] = i-1;
		++k;
		if (dd == DIRECT) {
			i2[k] = i;
			i1[k] = i-1;
			++k;
		}
		i1[k] = i*L;
		i2[k] = i*L-L;
		++k;
		if (dd == DIRECT) {
			i2[k] = i*L;
			i1[k] = i*L-L;
			++k;
		}
	}
	if (cc == CYCLE && L != 2) {
		for (i=0; i<L; ++i) {
			i1[k] = i;
			i2[k] = i+(L-1)*L;
			++k;
			if (dd == DIRECT) {
				i2[k] = i;
				i1[k] = i+(L-1)*L;
				++k;
			}
			i1[k] = i*L;
			i2[k] = i*L+L-1;
			++k;
			if (dd == DIRECT) {
				i2[k] = i*L;
				i1[k] = i*L+L-1;
				++k;
			}
		}
	}
	assert(linesNum == k);

	file->i1 = i1;
	file->i2 = i2;
	file->linesNum = linesNum;
	file->memNum = linesNum;
	print2l("%s =>> ......end.\n", __func__);
	return file;
}

/**
 * for 1D Line, N is number of nodes in the line, N must > 1
 *
 * N = 2 is a special case. total linesNum:
 * 		cycle,		direct: 2
 * 		cycle,	non_direct: 1
 * 	non_cycle,		direct: 2
 * 	non_cycle, 	non_direct: 1
 * 	
 * when N > 2, total linesNum:
 * 		cycle, 		direct: N*2
 * 		cycle, 	non_direct: N
 * 	non_cycle, 		direct: (N-1)*2
 * 	non_cycle,	non_direct: N-1
 * 
 */
struct LineFile * line1d_DS(int N, enum CICLENET cc, enum DIRECTNET dd) {
	print2l("%s =>> begin......\n", __func__);
	if (N<2) isError("%s =>> N too small.\n", __func__);
	
	struct LineFile *file = create_LineFile(NULL);

	long linesNum = N-1;
	if (cc == CYCLE && N != 2) {
		linesNum += 1;
	}
	if (dd == DIRECT) {
		linesNum *= 2;
	}

	int *i1 = malloc(linesNum * sizeof(int));
	int *i2 = malloc(linesNum * sizeof(int));

	int i;
	long k=0;
	for (i=1; i<N; ++i) {
		i1[k] = i;
		i2[k] = i-1;
		++k;
		if (dd == DIRECT) {
			i2[k] = i;
			i1[k] = i-1;
			++k;
		}
	}
	if (cc == CYCLE && N != 2) {
		i1[k] = 0;
		i2[k] = N - 1;
		++k;
		if (dd == DIRECT) {
			i2[k] = 0;
			i1[k] = N - 1;
			++k;
		}
	}

	assert(linesNum == k);

	file->i1 = i1;
	file->i2 = i2;
	file->linesNum = linesNum;
	file->memNum = linesNum;
	print2l("%s =>> ......end.\n", __func__);
	return file;
}

void parts45_DS(char *filename, long linesNum, int in, int dn, int cn, int ln, int ccn) {
	print2l("%s =>> begin......\n", __func__);
	FILE *fp = fopen(filename, "w");
	fileError(fp, "parts45_DS");
	long i;
	int j;
	set_timeseed_MTPR();
	for (i = 0; i < linesNum; ++i) {
		for (j = 0; j < in; ++j) {
			fprintf(fp, "%d\t", (int)get_i31_MTPR());
		}
		for (j = 0; j < dn; ++j) {
			fprintf(fp, "%f\t", get_d01_MTPR() + get_i31_MTPR()%100000);
		}
		for (j = 0; j < cn; ++j) {
			fprintf(fp, "%d\t", (int)get_i31_MTPR()%128);
		}
		for (j = 0; j < ln; ++j) {
			fprintf(fp, "%ld\t", get_i31_MTPR());
		}
		for (j = 0; j < ccn; ++j) {
			fprintf(fp, "%d%c%d\t", (int)get_i31_MTPR(), (int)get_i31_MTPR()%26 + 65, (int)get_i31_MTPR()%128);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	print2l("%s =>> ......end.\n", __func__);
}

/**
 * this function generate a ER random network.
 * N is the nodes number, it has to be larger than 9.(actually it should be larger than 100, that make more sence)
 * seed is the MT PRNG seed.
 *
 * I set p = 3.0*log(N)/N, so that the network will be full connected in most and most cases.(but not 100%)
 *
 */
struct LineFile *ER_DS(int N, int seed) {
	print2l("%s =>> begin......\n", __func__);
	if (N < 10) {
		isError("ER_DS N is too small");
	}
	set_seed_MTPR(seed);
	double p = 3.0*log(N)/N;
	long L = (long)(p*N*(N-1)/2.0);
	int *id1 = smalloc(L*sizeof(int));
	int *id2 = smalloc(L*sizeof(int));
	long linesNum = 0;
	long memNum = L;
	long STP = 0.1 * L;
	int i, j;
	for (i = 0; i < N; ++i) {
		for (j = i+1; j < N; ++j) {
			if (get_d01_MTPR() < p) {
				id1[linesNum] = i;
				id2[linesNum] = j;
				++linesNum;
				if (linesNum == memNum) {
					id1 = srealloc(id1, (memNum + STP)*sizeof(int));
					id2 = srealloc(id2, (memNum + STP)*sizeof(int));
					memNum += STP;
				}
			}
		}
	}
	struct LineFile *lf = create_LineFile(NULL);
	lf->i1 = id1;
	lf->i2 = id2;
	lf->linesNum = linesNum;
	lf->memNum = memNum;
	lf->filename = "ER random network";
	print3l("%s =>> Generate ER random network, N: %d, linesNum: %ld, memNum: %ld\n", __func__, N, linesNum, memNum);
	print2l("%s =>> ......end.\n", __func__);
	return lf;
}

/**
 * this function generate scale-free network using BA model.
 * N is the nodes number, it has to be larger than 9.(actually it should be larger than 100, that make more sence)
 * seed is the MT PRNG seed.
 * MM0 has to be larger than 0.
 *
 * MM0 is the m in BA model. it decides the edges number of network.
 * I am not sure about the whether the network is full connected or not, but it looks like in 
 * most cases the generated network is full connected.
 *
 */
struct LineFile *SF_DS(int N, int seed, int MM0) {
	print2l("%s =>> begin......\n", __func__);
	if (N < 10) {
		isError("SF_DS N is too small");
	}
	if (MM0 < 1) {
		isError("SF_DS MM0 is too small");
	}
	set_seed_MTPR(seed);

	int m = MM0, m0 = MM0 + 10;
	long memNum = (long)m*(N-m0) + m0;

	int *id1 = smalloc(memNum * sizeof(int));
	int *id2 = smalloc(memNum * sizeof(int));
	int i;
	for (i = 0; i < m0 - 1; ++i) {
		id1[i] = i;
		id2[i] = i+1;
	}
	id1[m0 - 1] = m0 - 1;
	id2[m0 - 1] = 0;
	long linesNum = m0;

	long *count = calloc(N, sizeof(long));
	for (i = 0; i < m0; ++i) {
		count[i] = 2;
	}
	long countSum = m0*2;
	int maxId = m0 - 1;
	char *fg = calloc(N, sizeof(char));
	int *fgM = malloc(m*sizeof(int));
	int j;
	while(maxId != N -1 && linesNum < memNum) {
		maxId++;
		for (j = 0; j < m; ) {
			int i2 = get_i31_MTPR()%countSum;
			int k = 0;
			for (i = 0; i < maxId; ++i) {
				k += count[i];
				if (k>i2 && fg[i] == 0) {
					fgM[j++] = i;
					fg[i] = 1;
					break;
				}
			}
		}
		for (j = 0; j < m; ++j) {
			int i2 = fgM[j];
			id1[linesNum + j] = maxId;
			id2[linesNum + j] = i2;
			count[i2]++;
			count[maxId]++;
			countSum += 2;
			fg[i2] = 0;
		}
		linesNum += m;
	}
	free(fg);
	free(fgM);
	free(count);

	struct LineFile *lf = create_LineFile(NULL);
	lf->i1 = id1;
	lf->i2 = id2;
	lf->linesNum = linesNum;
	lf->memNum = memNum;
	lf->filename = "ER random network";
	print3l("%s =>> Generate SF network, N: %d, linesNum: %ld, memNum: %ld.\n", __func__, N, linesNum, memNum);fflush(stdout);
	print2l("%s =>> ......end.\n", __func__);
	return lf;
}
