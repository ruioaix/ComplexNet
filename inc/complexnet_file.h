#ifndef COMPLEXNET_FILE_H
#define COMPLEXNET_FILE_H

// for strtol, strtok
#include <string.h>

// for FILE fpos_t 
#include <stdio.h>

// for malloc, 
#include <stdlib.h>

// for assert.
#include <assert.h>

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
#define LINES_DAY 100000

struct LineInfo{
	int vt1Id;
	int vt2Id;
//	int time;
//	int weight;
};

struct NetFile{
	int maxId;
	int minId;
	int linesNum;
	struct LineInfo *lines;
}

//the following two functions is for reading file. 
//fillLineInfo fill a line from a file.
void fillLineInfo(char *line, struct LineInfo *LI, int *maxId, int *minId);
//for each line, readFileLBL call fillLineInfo to fill the LineInfo.
//when finish, you get the edNum, vtMaxId, vtMinId and all information in LineInfo.
//partNum means how many parts a line contains. 
//the function can handle arbitrary partNum, you only need to change partNum, and modify LineInfo and fillLineInfo.
//you don't need to change anything in readFileLBL function.
//this function can handle file with arbitrary size, but maybe you need to think about physical memory in your PC.
//the member in LineInfo is correspond to line's different part.
//for line which is blank or contains less than partNum parts, the line will be ignored.
//for line which contains more than partNum parts, only partNum parts will be read.
struct NetFile *readFileLBL(char *filename);

#endif
