#ifndef I3LINEFILE_H
#define I3LINEFILE_H

struct i3Line{
	int i1;
	int i2;
	int i3;
};
struct i3LineFile{
	int i1Max;
	int i1Min;
	int i2Max;
	int i2Min;
	long linesNum;
	struct i3Line *lines;
};
void free_i3LineFile(struct i3LineFile *file);
struct i3LineFile *create_i3LineFile(const char * const filename);
void print_i3LineFile(struct i3LineFile *file, char *filename);
int geti1num_i3LineFile(struct i3LineFile *file);

#endif
