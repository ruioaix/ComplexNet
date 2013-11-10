#ifndef I5DSLINEFILE
#define I5DSLINEFILE

//for file containing 5 int, char *, double
struct i5dsLine {
	int i1;
	int i2;
	int i3;
	int i4;
	int i5;
	double  d6;
	char s7[1000];
};
struct i5dsLineFile {
	int i1Max;
	int i1Min;
	int i2Max;
	int i2Min;
	long linesNum;
	struct i5dsLine *lines;
};
void free_i5dsLineFile(struct i5dsLineFile *file);
struct i5dsLineFile *create_i5dsLineFile(const char *const filename);

#endif
