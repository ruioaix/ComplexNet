#include "generatenet.h"
#include "error.h"
#include <stdlib.h>

struct iiLineFile * generateNet_2D(int L, enum CICLENET cc) {
	struct iiLineFile *file = malloc(sizeof(struct iiLineFile));
	long linesNum = (L-1)*L*2;
	if (cc == cycle) {
		linesNum += 2*L;
	}

	struct iiLine *lines = malloc(linesNum*sizeof(struct iiLine));
	int i,j;
	long k=0;
	for (i=1; i<L; ++i) {
		for (j=1; j<L; ++j) {
			int id = i*L + j;
			lines[k].i1=id;
			lines[k].i2=id-1;	
			++k;
			lines[k].i1=id;
			lines[k].i2=id-L;
			++k;
		}
	}
	for (i=1; i<L; ++i) {
		lines[k].i1 = i;
		lines[k].i2 = i-1;
		++k;
		lines[k].i1 = i*L;
		lines[k].i2 = i*L-L;
		++k;
	}
	if (cc == cycle) {
		for (i=1; i<L; ++i) {
			lines[k].i1 = i;
			lines[k].i2 = i+(L-1)*L;
			++k;
			lines[k].i1 = i*L;
			lines[k].i2 = i*L+L-1;
			++k;
		}
		lines[k].i1=0;
		lines[k].i2=L-1;
		++k;
		lines[k].i1=0;
		lines[k].i2=(L-1)*L;
		++k;
	}
	if(linesNum != k) {
		printf("%ld\t%ld\n", linesNum, k);
		isError("generateNet_2D");
	}
		printf("%ld\t%ld\n", linesNum, k);
	file->lines = lines;
	file->linesNum = linesNum;
	file->i1Max = L*L - 1;
	file->i1Min = 1;
	file->i2Max = L*L - 2;
	file->i2Min = 0;
	if (cc == cycle) {
		file->i1Min = 0;
		file->i2Max = L*L - 1;
	}
	return file;
}

struct iiLineFile * generateNet_1D(int L, enum CICLENET cc) {
	struct iiLineFile *file = malloc(sizeof(struct iiLineFile));

	long linesNum = L-1;
	if (cc == cycle) {
		linesNum += 1;
	}

	struct iiLine *lines = malloc(linesNum*sizeof(struct iiLine));
	int i;
	long k=0;
	for (i=1; i<L; ++i) {
		lines[k].i1 = i;
		lines[k].i2 = i-1;
		++k;
	}
	if (cc == cycle) {
		lines[k].i1 = 0;
		lines[k].i2 = L - 1;
		++k;
	}

	if(linesNum != k) {
		printf("%ld\t%ld\n", linesNum, k);
		isError("generateNet_1D");
	}

	file->lines= lines;
	file->linesNum = linesNum;
	file->i1Max = L-1;
	file->i1Min = 0;
	file->i2Max = L-1;
	file->i2Min = 0;
	return file;
}
