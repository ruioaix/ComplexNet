#ifndef IIDLINEFILE
#define IIDLINEFILE

//for file containing int/double/int 
struct idiLine{
	int i1;
	double d2;
	int i3;
};
struct idiLineFile {
	int iMax;
	int iMin;
	long linesNum;
	struct idiLine *lines;
};
void free_idiLineFile(struct idiLineFile *file);
struct idiLineFile *create_idiLineFile(const char *const filename);

#endif
