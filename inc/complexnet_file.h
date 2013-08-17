#ifndef COMPLEXNET_FILE_H
#define COMPLEXNET_FILE_H

// for strtol, strtok
#include <string.h>

// for FILE fpos_t 
#include <stdio.h>

// for malloc, remalloc
#include <stdlib.h>

// for assert.
#include <assert.h>

// for INT_MAX
#include <limits.h>

// for fileError, memError, isError
#include "complexnet_error.h"

//if define VEXTER_FILE_DIRECTION_LEFT, data file's column 2 point to column 1, direction is left.
//if not define, column 1 points column 2. that's normal.
//#define VEXTER_FILE_DIRECTION_LEFT

//LINE_LENGTH_MAX is a serious constant, you should be sure a line's length not exceed this value.
#define LINE_LENGTH_MAX 100000

//LINES_LENGTH_EACH is the stepLength. 
//now it's 1e7, means, if a file contains less than 1e7 lines, malloc will be called only one time.
//if a file contans 1e8 lines, malloc will be called ten times.
//of course, if a file contains 1e8 lines, maybe you want to set LINES_LENGTH_EACH to 5e7 or 1e8. that's depend on you.
//you don't need to know the exactly line num of the file.
#define LINES_LENGTH_EACH 10000000

//if the net's vt id beyond the INT_MAX, or the line num beyond the INT_MAX, you can just change int to long. if you like, long long is also fine.
//but int is suggested, because it's fastest.
//the edtype is for edges number in net, it's common to be 1e8, so int is ok, too. 
typedef int vttype;
typedef int edtype;


//for net file
struct NetLineInfo{
	vttype vt1Id;
	vttype vt2Id;
};
struct NetFile{
	vttype maxId;
	vttype minId;
	edtype linesNum;
	struct NetLineInfo *lines;
};
void freeNetFile(struct NetFile *file);
void fillNetLineInfo(char *line, struct NetLineInfo *LI_origin,  edtype *lNum, int each, vttype *vtMaxId, vttype *vtMinId);
struct NetFile *readNetFile(const char * const filename);


//for is file
struct InfectSource {
	vttype num;
	vttype *vt;
};
struct InfectSourceFile {
	int ISsNum;
	struct InfectSource *lines;
};
void freeISFile(struct InfectSourceFile *file);
struct InfectSource fillISfromLine(char *line);
struct InfectSourceFile *readAllISfromFile(const char * const filename);

#endif
