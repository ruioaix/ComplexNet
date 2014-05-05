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
	if (L<2) isError("lattice2d_DS's L too small");

	struct LineFile *file = create_LineFile(NULL);
	long linesNum = (L-1)*L*2;
	if (cc == CYCLE && L != 2) {
		linesNum += 2*L;
	}
	if (dd == DIRECT) {
		linesNum *= 2;
	}
	printf("Generate 2D Lattice => %s, %s, L: %d, linesNum: %ld\n", CICLENETC[cc], DIRECTNETC[dd], L, linesNum);fflush(stdout);

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
	if (N<2) isError("line1d_DS's N too small");
	
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
	return file;
}

void parts45_DS(char *filename, long linesNum, int in, int dn, int cn, int ln, int ccn) {
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
}

struct LineFile *ER_DS(int N, int seed) {
	assert(N>2);
	set_seed_MTPR(seed);
	double p = 3.0*log(N)/N;
	long L = (long)(p*N*N);
	int *id1 = smalloc(L*sizeof(int));
	int *id2 = smalloc(L*sizeof(int));
	long linesNum = 0;
	long memNum = L;
	int i, j;
	for (i = 0; i < N; ++i) {
		for (j = i+1; j < N; ++j) {
			if (get_d01_MTPR() < p) {
				id1[linesNum] = i;
				id2[linesNum] = j;
				++linesNum;
				if (linesNum == memNum) {
					srealloc(id1, (memNum + L)*sizeof(int));
					srealloc(id2, (memNum + L)*sizeof(int));
					memNum += L;
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
	printf("Generate ER random network => N: %d, linesNum: %ld, memNum: %ld\n", N, linesNum, memNum);fflush(stdout);
	return lf;
}

#define MM0 11
struct LineFile *SF_DS(int N, int seed) {
	assert(N>2);
	set_seed_MTPR(seed);
	int m, m0;
	m = m0 = MM0;
	long linesNum = 0;
	long memNum = (long)m*N + MM0;
	int *id1 = smalloc(memNum * sizeof(int));
	int *id2 = smalloc(memNum * sizeof(int));
	int i;
	for (i = 0; i < MM0 - 1; ++i) {
		id1[i] = i;
		id2[i] = i+1;
	}
	id1[MM0 - 1] = MM0 - 1;
	id2[MM0 - 1] = 0;
	linesNum = MM0;
	long *count = calloc(N, sizeof(long));
	for (i = 0; i < MM0; ++i) {
		count[i] = 2;
	}
	long countSum = MM0*2;
	int maxId = MM0 - 1;
	char *fg = calloc(N, sizeof(char));
	int fgM[MM0];
	int j;
	while(maxId != N -1 && linesNum + MM0 < memNum) {
		maxId++;
		for (j = 0; j < MM0; ) {
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
		for (j = 0; j < MM0; ++j) {
			int i2 = fgM[j];
			//printf("%d\t%d\n", j, i2);
			id1[linesNum + j] = maxId;
			id2[linesNum + j] = i2;
			//print_label(i2);
			count[i2]++;
			count[maxId]++;
			countSum += 2;
			fg[i2] = 0;
		}
		linesNum += MM0;
	}
	free(fg);
	free(count);
	struct LineFile *lf = create_LineFile(NULL);
	lf->i1 = id1;
	lf->i2 = id2;
	lf->linesNum = linesNum;
	lf->memNum = memNum;
	lf->filename = "ER random network";
	printf("Generate SF network => N: %d, linesNum: %ld, memNum: %ld\n", N, linesNum, memNum);fflush(stdout);
	return lf;
}
