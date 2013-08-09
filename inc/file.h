#ifndef FILE_H
#define FILE_H

// for strtol, strtok
#include <string.h>

// for FILE fpos_t 
#include <stdio.h>

// for malloc, 
#include <stdlib.h>

// for fileError, memError, isError
#include "errorhandle.h"

#include "sort.h"

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
	long vt1Id;
	long vt2Id;
	int time;
	int weight;
};

//the following two functions is for reading file. 
//fillLineInfo fill a line from a file.
long fillLineInfo(char *line, int partNum, struct LineInfo *LI, long *vtMaxId, long *vtMinId);
//for each line, readFileLBL call fillLineInfo to fill the LineInfo.
//when finish, you get the edNum, vtMaxId, vtMinId and all information in LineInfo.
//partNum means how many parts a line contains. 
//the function can handle arbitrary partNum, you only need to change partNum, and modify LineInfo and fillLineInfo.
//you don't need to change anything in readFileLBL function.
//this function can handle file with arbitrary size, but maybe you need to think about physical memory in your PC.
//the member in LineInfo is correspond to line's different part.
//for line which is blank or contains less than partNum parts, the line will be ignored.
//for line which contains more than partNum parts, only partNum parts will be read.
struct LineInfo *readFileLBL(char *filename, long *edNum, long *vtMaxId, long *vtMinId, int partNum);


//the following four functions is for change time complex network's time granularity.
//getDaysLines get the lines which time is >=daystart, <dayend.
struct LineInfo *getDaysLines(struct LineInfo *edList, int edNum, int daystart, int dayend, int *dayNum);
//change time granularity, it's complicate, but it's right.
struct LineInfo *AddWeight2LineInfo(struct LineInfo *dayList, int dayNum, int *realdayNum, int daystepNum);
void moveLI2LIW(int *vt1, int *vt2, int begin, int end, struct LineInfo *dayListWeight, int *realdayNum);

#endif
