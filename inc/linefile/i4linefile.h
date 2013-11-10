#ifndef I4LINEFILE
#define I4LINEFILE

//for file containing four int in one line.
struct i4Line {
	int i1;
	int i2;
	int i3;
	int i4;
};
struct i4LineFile {
	int i1Max;
	int i1Min;
	int i2Max;
	int i2Min;
	long linesNum;
	struct i4Line *lines;
};
void free_i4LineFile(struct i4LineFile *file);
void free_2_i4LineFile(struct i4LineFile *twofile);
struct i4LineFile *create_i4LineFile(const char * const filename);
void print_i4LineFile(struct i4LineFile *file, char *filename);
void print_2_i4LineFile(struct i4LineFile *file1, struct i4LineFile *file2, char *filename);
struct i4LineFile *divide_i4LineFile(struct i4LineFile *file, double rate);

#endif
