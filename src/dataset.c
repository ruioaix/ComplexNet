#include "base.h"
#include "dataset.h"
#include <stdlib.h>
#include <assert.h>

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
