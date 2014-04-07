#include "common.h"
#include "generateds.h"
#include <stdlib.h>
#include <assert.h>

static char *CICLENETC[2] = {"cycle", "non_cycle"};
static char *DIRECTNETC[2] = {"direct", "non_direct"};

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
struct iiLineFile * generate_2DLattice(int L, enum CICLENET cc, enum DIRECTNET dd) {
	if (L<2) isError("generate_2DLattice L too small");
	struct iiLineFile *file = malloc(sizeof(struct iiLineFile));
	long linesNum = (L-1)*L*2;
	if (cc == cycle && L != 2) {
		linesNum += 2*L;
	}
	if (dd == direct) {
		linesNum *= 2;
	}
	printf("Generate 2D Lattice => %s, %s, L: %d, linesNum: %ld\n", CICLENETC[cc], DIRECTNETC[dd], L, linesNum);fflush(stdout);

	struct iiLine *lines = malloc(linesNum*sizeof(struct iiLine));
	int i,j;
	long k=0;
	for (i=1; i<L; ++i) {
		for (j=1; j<L; ++j) {
			int id = i*L + j;
			lines[k].i1=id;
			lines[k].i2=id-1;	
			++k;
			if (dd == direct) {
				lines[k].i2=id;
				lines[k].i1=id-1;	
				++k;
			}
			lines[k].i1=id;
			lines[k].i2=id-L;
			++k;
			if (dd == direct) {
				lines[k].i2=id;
				lines[k].i1=id-L;
				++k;
			}
		}
	}
	for (i=1; i<L; ++i) {
		lines[k].i1 = i;
		lines[k].i2 = i-1;
		++k;
		if (dd == direct) {
			lines[k].i2 = i;
			lines[k].i1 = i-1;
			++k;
		}
		lines[k].i1 = i*L;
		lines[k].i2 = i*L-L;
		++k;
		if (dd == direct) {
			lines[k].i2 = i*L;
			lines[k].i1 = i*L-L;
			++k;
		}
	}
	if (cc == cycle && L != 2) {
		for (i=0; i<L; ++i) {
			lines[k].i1 = i;
			lines[k].i2 = i+(L-1)*L;
			++k;
			if (dd == direct) {
				lines[k].i2 = i;
				lines[k].i1 = i+(L-1)*L;
				++k;
			}
			lines[k].i1 = i*L;
			lines[k].i2 = i*L+L-1;
			++k;
			if (dd == direct) {
				lines[k].i2 = i*L;
				lines[k].i1 = i*L+L-1;
				++k;
			}
		}
	}
	assert(linesNum == k);

	file->lines = lines;
	file->linesNum = linesNum;
	file->i1Max = L*L - 1;
	file->i1Min = 1;
	file->i2Max = L*L - 2;
	file->i2Min = 0;
	if ((cc == cycle && L != 2)|| dd == direct) { 
		file->i1Min = 0;
		file->i2Max = L*L - 1;
	}
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
struct iiLineFile * generate_1DLine(int N, enum CICLENET cc, enum DIRECTNET dd) {
	if (N<2) isError("generate_1DLine N too small");
	
	struct iiLineFile *file = malloc(sizeof(struct iiLineFile));

	long linesNum = N-1;
	if (cc == cycle && N != 2) {
		linesNum += 1;
	}
	if (dd == direct) {
		linesNum *= 2;
	}

	struct iiLine *lines = malloc(linesNum*sizeof(struct iiLine));
	int i;
	long k=0;
	for (i=1; i<N; ++i) {
		lines[k].i1 = i;
		lines[k].i2 = i-1;
		++k;
		if (dd == direct) {
			lines[k].i2 = i;
			lines[k].i1 = i-1;
			++k;
		}
	}
	if (cc == cycle && N != 2) {
		lines[k].i1 = 0;
		lines[k].i2 = N - 1;
		++k;
		if (dd == direct) {
			lines[k].i2 = 0;
			lines[k].i1 = N - 1;
			++k;
		}
	}

	assert(linesNum == k);

	file->lines= lines;
	file->linesNum = linesNum;
	file->i1Max = N-1;
	file->i1Min = 1;
	file->i2Max = N-2;
	file->i2Min = 0;
	if (dd == direct || (cc == cycle && N !=2)) {
		file->i1Min = 0;
		file->i2Max = N -1;
	}
	return file;
}
