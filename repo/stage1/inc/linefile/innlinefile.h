#ifndef INNLINEFILE
#define INNLINEFILE

//for file containing unlimited int in one line.
struct innLine{
	int lineId;
	int num;
	int *inn;
};
struct innLineFile {
	long linesNum;
	struct innLine *lines;
};
void free_innLineFile(struct innLineFile *file);
struct innLineFile *create_innLineFile(const char * const filename);

#endif
