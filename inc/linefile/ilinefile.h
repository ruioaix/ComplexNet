#ifndef ILINEFILE
#define ILINEFILE

// for file containing one int in one line.
struct iLine {
	int i1;
};
struct iLineFile {
	int i1Max;
	int i1Min;
	long linesNum;
	struct iLine *lines;
};
void free_iLineFile(struct iLineFile *file);
struct iLineFile *create_iLineFile(const char * const filename);
void print_iLineFile(struct iLineFile *file, char *filename);

#endif
