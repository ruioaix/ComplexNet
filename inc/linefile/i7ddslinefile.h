#ifndef I7DDSLINEFILE
#define I7DDSLINEFILE

//for file containing int/int/int/int/int/int/int/double/double
struct i7ddsLine {
	int i1;
	int i2;
	int i3;
	int i4;
	int i5;
	int i6;
	int i7;
	double d8;
	double d9;
	char s10[100];
};
struct i7ddsLineFile {
	int iMax;
	int iMin;
	long linesNum;
	struct i7ddsLine *lines;
};
void free_i7ddsLineFile(struct i7ddsLineFile *file);
struct i7ddsLineFile *create_i7ddsLineFile(const char *const filename);

#endif
