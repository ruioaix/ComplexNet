#ifndef CN_LINEFILE_H
#define CN_LINEFILE_H

struct LineFile {
	int *i1;
	int i1Max;
	int i1Min;

	int *i2;
	int i2Max;
	int i2Min;

	int *d1;
	int d1Max;
	int d2Min;

	long linesNum;
};

struct LineFile *create_LineFile(const char * const filename, ...);

#endif
