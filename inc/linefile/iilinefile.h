//for file containing two int in one line.
struct iiLine {
	int i1;
	int i2;
};
struct iiLineFile {
	int iMax;
	int iMin;
	long linesNum;
	struct iiLine *lines;
};
void free_iiLineFile(struct iiLineFile *file);
struct iiLineFile *create_iiLineFile(const char * const filename);
