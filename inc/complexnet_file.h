#ifndef COMPLEXNET_FILE_H
#define COMPLEXNET_FILE_H

//if define VEXTER_FILE_DIRECTION_LEFT, data file's column 2 point to column 1, direction is left.
//if not define, column 1 points column 2. that's normal.
#define VEXTER_FILE_DIRECTION_LEFT

//LINE_LENGTH_MAX is a serious constant, you should be sure a line's length not exceed this value.
#define LINE_LENGTH_MAX 100000

//LINES_LENGTH_EACH is the stepLength. 
//now it's 1e7, means, if a file contains less than 1e7 lines, malloc will be called only one time.
//if a file contans 1e8 lines, malloc will be called ten times.
//of course, if a file contains 1e8 lines, maybe you want to set LINES_LENGTH_EACH to 5e7 or 1e8. that's depend on you.
//you don't need to know the exactly line num of the file.
#define LINES_LENGTH_EACH 10000000

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

//for file containing four int in one line.
struct i4Line {
	int i1;
	int i2;
	int i3;
	int i4;
};
struct i4LineFile {
	int iMax;
	int iMin;
	long linesNum;
	struct i4Line *lines;
};
void free_i4LineFile(struct i4LineFile *file);
struct i4LineFile *create_i4LineFile(const char * const filename);

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
