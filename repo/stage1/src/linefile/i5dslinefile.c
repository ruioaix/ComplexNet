#include "../../inc/linefile/i5dslinefile.h"
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

static void fill_i5dsLine(char *line, struct i5dsLine *LI_origin,  long *lNum, int each, int *vtMaxId, int *vtMinId, int *_vtMaxId, int *_vtMinId, long filelineNum)
{
	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %ld has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", filelineNum, LINE_LENGTH_MAX-1);
		return;
	}

	long linesNum = *lNum+(each-1)*LINES_LENGTH_EACH;
	struct i5dsLine *LI = LI_origin+linesNum;

	//divide line to parts.
	//strtok return a c string(end with a '\0').
	char *delimiter="\t,/\r\n";
	char *partsLine[7];
	partsLine[0]=strtok(line, delimiter);
	if (partsLine[0]==NULL) {
		printf("\tline %ld not valid, ignored (looks like a blank line).\n", filelineNum);
		return;
	}
	partsLine[1]=strtok(NULL, delimiter);
	if (partsLine[1]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only one number)\n", filelineNum);
		return;
	}
	partsLine[2]=strtok(NULL, delimiter);
	if (partsLine[2]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only two number)\n", filelineNum);
		return;
	}
	partsLine[3]=strtok(NULL, delimiter);
	if (partsLine[3]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only three number)\n", filelineNum);
		return;
	}
	partsLine[4]=strtok(NULL, delimiter);
	if (partsLine[4]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only four number)\n", filelineNum);
		return;
	}
	partsLine[5]=strtok(NULL, delimiter);
	if (partsLine[5]==NULL) {
		printf("\tline %ld not valid, ignored (looks like the last double number)\n", filelineNum);
		return;
	}
	partsLine[6]=strtok(NULL, delimiter);
	if (partsLine[6]==NULL) {
		printf("\tline %ld not valid, ignored (looks like no string contain)\n", filelineNum);
		return;
	}

	//transfor parts to num(double or int);
	char *pEnd;
	LI->i1=strtol(partsLine[0], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	LI->i2=strtol(partsLine[1], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	LI->i3=strtol(partsLine[2], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	LI->i4=strtol(partsLine[3], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	LI->i5=strtol(partsLine[4], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	LI->d6=strtod(partsLine[5], &pEnd);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}

	strncpy(LI->s7, partsLine[6], sizeof(LI->s7));

	++(*lNum);

	//max/min Id
	*vtMaxId=(*vtMaxId)>LI->i1?(*vtMaxId):LI->i1;
	*vtMinId=(*vtMinId)<LI->i1?(*vtMinId):LI->i1;
	*_vtMaxId=(*_vtMaxId)>LI->i2?(*_vtMaxId):LI->i2;
	*_vtMinId=(*_vtMinId)<LI->i2?(*_vtMinId):LI->i2;
}

struct i5dsLineFile *create_i5dsLineFile(const char * const filename)
{
	printf("read i5dsLineFile %s: \n", filename);
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);

	struct i5dsLine *LinesInfo=NULL;
	LinesInfo=malloc(LINES_LENGTH_EACH*sizeof(struct i5dsLine));
	assert(LinesInfo!=NULL);

	long linesNum=0;
	long filelineNum=0;
	int maxId=-1;
	int minId=INT_MAX;
	int _maxId = -1;
	int _minId = INT_MAX;

	char line[LINE_LENGTH_MAX];
	int each=1;
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		++filelineNum;
		if (linesNum<LINES_LENGTH_EACH) {
			fill_i5dsLine(line, LinesInfo, &linesNum, each, &maxId, &minId, &_maxId, &_minId, filelineNum);
		} else {
			++each;
			printf("\tread valid lines: %d\n", (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct i5dsLine *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct i5dsLine));
			assert(temp!=NULL);
			LinesInfo=temp;
			linesNum=0;
			//fill_i5dsLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
			fill_i5dsLine(line, LinesInfo, &linesNum, each, &maxId, &minId, &_maxId, &_minId, filelineNum);
		}
	}
	linesNum+=(each-1)*LINES_LENGTH_EACH;
	printf("\tread valid lines: %ld, file lines: %ld\n\tMax: %d, Min: %d\n", linesNum, filelineNum, maxId, minId); fflush(stdout);
	fclose(fp);

	struct i5dsLineFile *file=malloc(sizeof(struct i5dsLineFile));
	assert(file!=NULL);
	file->i1Min=minId;
	file->i1Max=maxId;
	file->i2Min = _minId;
	file->i2Max = _maxId;
	file->lines=LinesInfo;
	file->linesNum=linesNum;

	return file;
}
void free_i5dsLineFile(struct i5dsLineFile *file) {
	if(file != NULL) {
		free(file->lines);
		free(file);
	}
}
