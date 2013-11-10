#ifndef IID3LINEFILE
#define IID3LINEFILE

//for file containing int/int/double/double/double
struct iid3Line {
	int i1;
	int i2;
	double d3;
	double d4;
	double d5;
};
struct iid3LineFile {
	int iMax;
	int iMin;
	long linesNum;
	struct iid3Line *lines;
};
void free_iid3LineFile(struct iid3LineFile *file);
struct iid3LineFile *create_iid3LineFile(const char * const filename);

#endif
