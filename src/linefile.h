#ifndef CN_LINEFILE_H
#define CN_LINEFILE_H

struct LineFile {
	int *i1;
	int i1Max;
	int i1Min;

	int *i2;
	int i2Max;
	int i2Min;

	int *i3;
	int i3Max;
	int i3Min;

	int *i4;
	int i4Max;
	int i4Min;

	int *i5;
	int i5Max;
	int i5Min;

	int *i6;
	int i6Max;
	int i6Min;

	int *i7;
	int i7Max;
	int i7Min;

	int *i8;
	int i8Max;
	int i8Min;

	int *i9;
	int i9Max;
	int i9Min;

	double *d1;
	double d1Max;
	double d1Min;

	double *d2;
	double d2Max;
	double d2Min;

	double *d3;
	double d3Max;
	double d3Min;

	double *d4;
	double d4Max;
	double d4Min;

	double *d5;
	double d5Max;
	double d5Min;

	double *d6;
	double d6Max;
	double d6Min;

	double *d7;
	double d7Max;
	double d7Min;

	double *d8;
	double d8Max;
	double d8Min;

	double *d9;
	double d9Max;
	double d9Min;

	long linesNum;
	long memNum;
};

struct LineFile *create_LineFile(const char * const filename, ...);
void print_LineFile(struct LineFile *lf, char *filename);

#endif
