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

#include "complexnet_sort.h"

//LINE_LENGTH_MAX is a serious constant, you should be sure a line's length not exceed this value.
#define LINE_LENGTH_MAX 10000
//LINES_LENGTH_EACH is the stepLength. 
//now it's 1e7, means, if a file contains less than 1e7 lines, malloc will be called only one time.
//if a file contans 1e8 lines, malloc will be called ten times.
//of course, if a file contains 1e8 lines, maybe you want to set LINES_LENGTH_EACH to 5e7 or 1e8. that's depend on you.
//you don't need to know the exactly line num of the file.
#define LINES_LENGTH_EACH 10000000

//if the net's vt id beyond the INT_MAX, or the line num beyond the INT_MAX, you can just change int to long. if you like, long long is also fine.
//but int is suggested, because it's fastest.
typedef int idtype;
typedef int linesnumtype;

struct LineInfo{
	idtype vt1Id;
	idtype vt2Id;
//	int time;
//	int weight;
};

struct NetFile{
	idtype maxId;
	idtype minId;
	linesnumtype linesNum;
	struct LineInfo *lines;
};

//the following two functions is for reading file. 
//fillLineInfo fill a line from a file into a struct LineInfo object.
void fillLineInfo(char *line, struct LineInfo *LI, idtype *maxId, idtype *minId);
//for each line, readFileLBL call fillLineInfo to fill the LineInfo.
//when finish, you get the linesNum, maxId, minId and all information in LineInfo.
//for lines containing different partNum, you only need to modify LineInfo and fillLineInfo.
//you don't need to change anything in readFileLBL function.
//this function can handle file with arbitrary size, the only limit you need to think about is physical memory in your PC.
//input is filename.
//output is struct NetFile.
struct NetFile *readFileLBL(char *filename);

#endif
