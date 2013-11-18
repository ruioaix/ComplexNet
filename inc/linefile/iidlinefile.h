#ifndef IIDLINEFILE_H
#define IIDLINEFILE_H

//for file containing int/double/int 
struct iidLine{
	int i1;
	int i2;
	double d3;
};
struct iidLineFile {
	int i1Max;
	int i1Min;
	int i2Max;
	int i2Min;
	long linesNum;
	struct iidLine *lines;
};
void free_iidLineFile(struct iidLineFile *file);
struct iidLineFile *create_iidLineFile(const char *const filename);
void print_iidLineFile(struct iidLineFile *file, char *filename);

#endif
