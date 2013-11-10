#ifndef I3DLINEFILE
#define I3DLINEFILE

//for file containing int,int, double,int
struct i3dLine{
	int i1;
	int i2;
	int i3;
	double d4;
};
struct i3dLineFile {
	int i1Max;
	int i1Min;
	int i2Max;
	int i2Min;
	long linesNum;
	struct i3dLine *lines;
};
void free_i3dLineFile(struct i3dLineFile *file);
struct i3dLineFile *create_i3dLineFile(const char *const filename);

#endif
