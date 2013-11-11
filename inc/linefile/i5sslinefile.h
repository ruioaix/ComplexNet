#ifndef I5SSLINEFILE_H
#define I5SSLINEFILE_H

//for file containing 5 int, char *, char *
struct i5ssLine {
	int i1;
	int i2;
	int i3;
	int i4;
	int i5;
	char s6[100];
	char s7[1000];
};
struct i5ssLineFile {
	int i1Max;
	int i1Min;
	int i2Max;
	int i2Min;
	long linesNum;
	struct i5ssLine *lines;
};
void free_i5ssLineFile(struct i5ssLineFile *file);
struct i5ssLineFile *create_i5ssLineFile(const char *const filename);

#endif
