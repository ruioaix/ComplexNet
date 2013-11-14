#ifndef I5LINEFILE_H
#define I5LINEFILE_H

struct i5Line{
	int i1;
	int i2;
	int i3;
	int i4;
	int i5;
};
struct i5LineFile{
	int i1Max;
	int i1Min;
	int i2Max;
	int i2Min;
	long linesNum;
	struct i5Line *lines;
};
void free_i5LineFile(struct i5LineFile *file);
struct i5LineFile *create_i5LineFile(const char * const filename);
void print_i5LineFile(struct i5LineFile *file, char *filename);

#endif
