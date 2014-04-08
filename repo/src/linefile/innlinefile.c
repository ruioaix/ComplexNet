#include "../../inc/linefile/innlinefile.h"
#include "../../inc/utility/error.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "limits.h"
#include "assert.h"

//LINE_LENGTH_MAX is a serious constant, you should be sure a line's length not exceed this value.
#define LINE_LENGTH_MAX 10000

//LINES_LENGTH_EACH is the stepLength. 
//now it's 1e7, means, if a file contains less than 1e7 lines, malloc will be called only one time.
//if a file contans 1e8 lines, malloc will be called ten times.
//of course, if a file contains 1e8 lines, maybe you want to set LINES_LENGTH_EACH to 5e7 or 1e8. that's depend on you.
//you don't need to know the exactly line num of the file.
#define LINES_LENGTH_EACH 1000000

//LINE_INT_NUM_EACH is another stepLength.
#define LINE_INT_NUM_EACH 5000


static struct innLine fill_innLine(char *line, long filelineNum) {
	struct innLine is;

	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %ld has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", filelineNum, LINE_LENGTH_MAX-1);
		is.num = 0;
		return is;
	}

	int isMax=LINE_INT_NUM_EACH;
	char **partsLine = calloc(isMax, sizeof(void *));
	char *delimiter="\t, \r\n:";
	partsLine[0]=strtok(line, delimiter);
	if (partsLine[0]==NULL) {
		is.num = 0;
		printf("\tline %ld not valid, ignored (looks like a blank line).\n", filelineNum);
		return is;
	}
	int i=0;
	while((partsLine[++i]=strtok(NULL, delimiter))) {
		if (i==isMax) {
			isMax += LINE_INT_NUM_EACH;
			char **temp = realloc(partsLine, isMax*sizeof(void *));
			assert(temp!=NULL);
			partsLine=temp;
		}
	}

	int num=i;
	int *vt=malloc(num*sizeof(int));
	int j=0;
	char *pEnd;
	for (j=0; j<num; ++j) {
		vt[j]=strtol(partsLine[j], &pEnd, 10);
		if (pEnd[0]!='\0') {
			free(vt);
			is.num = 0;
			printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
			free(partsLine);
			return is;
		}
	}

	free(partsLine);

	is.num=num;
	is.inn=vt;
	return is;
}

struct innLineFile *create_innLineFile(const char * const filename)
{
	printf("read innLineFile %s: \n", filename);
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);
	//record file begin position
	fpos_t file_position;
	fgetpos(fp, &file_position);

	struct innLineFile *isfile=malloc(sizeof(struct innLineFile));
	assert(isfile!=NULL);

	char line[LINE_LENGTH_MAX];
	long filelines=0;
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		++filelines;
	}
	assert(filelines!=0);

	isfile->lines = calloc(filelines, sizeof(struct innLine));
	assert(isfile->lines!=NULL);

	fsetpos(fp, &file_position);
	long linesNum=0;
	long filelineNum=0;
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		struct innLine is = fill_innLine(line, ++filelineNum);
		if (is.num!=0) {
			is.lineId=linesNum;
			isfile->lines[linesNum++]=is;
		}
	}
	fclose(fp);
	isfile->linesNum=linesNum;
	assert(linesNum!=0);
	printf("\ttotally %ld groups of IS, file lines: %ld\n", linesNum, filelines);

	return isfile;
}

void free_innLineFile(struct innLineFile *file) {
	long i;
	if (file != NULL) {
		for(i=0; i<file->linesNum; ++i) {
			free(file->lines[i].inn);
		}
		free(file->lines);
		free(file);
	}
}
