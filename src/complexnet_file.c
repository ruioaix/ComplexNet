#include "../inc/complexnet_file.h"
#include "../inc/complexnet_error.h" // for fileError, memError, isError
#include <string.h> // for strtol, strtok
#include <stdio.h> // for FILE fpos_t 
#include <stdlib.h> // for malloc, remalloc
#include <assert.h> // for assert.
#include <limits.h> // for INT_MAX


static char *delimiter="\t, \r\n";

void free_iiLineFile(struct iiLineFile *file) {
	if(file != NULL) {
		free(file->lines);
		free(file);
	}
}

//basically, for different line styled file, I only need to change this function and struct NetLineInfo.
static void fill_iiLine(char *line, struct iiLine *LI_origin,  long *lNum, int each, int *vtMaxId, int *vtMinId, long filelineNum)
{
	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %ld has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", filelineNum, LINE_LENGTH_MAX-1);
		return;
	}

	long linesNum = *lNum+(each-1)*LINES_LENGTH_EACH;
	struct iiLine *LI = LI_origin+linesNum;

	//divide line to parts.
	//strtok return a c string(end with a '\0').
	char *partsLine[2];
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

	++(*lNum);

	//max/min Id
	if (LI->i1>LI->i2) {
		*vtMaxId=(*vtMaxId)>LI->i1?(*vtMaxId):LI->i1;
		*vtMinId=(*vtMinId)<LI->i2?(*vtMinId):LI->i2;
	} else {
		*vtMaxId=(*vtMaxId)>LI->i2?(*vtMaxId):LI->i2;
		*vtMinId=(*vtMinId)<LI->i1?(*vtMinId):LI->i1;
	}
}

//if data is stored in each line and each line contain only num & delimiter, there is no need to change this function.
struct iiLineFile *create_iiLineFile(const char * const filename)
{
	printf("read iiLineFile %s: \n", filename);
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);

	struct iiLine *LinesInfo=NULL;
	LinesInfo=malloc(LINES_LENGTH_EACH*sizeof(struct iiLine));
	assert(LinesInfo!=NULL);

	long linesNum=0;
	long filelineNum=0;
	int maxId=-1;
	int minId=INT_MAX;

	char line[LINE_LENGTH_MAX];
	int each=1;
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		++filelineNum;
		if (linesNum<LINES_LENGTH_EACH) {
			fill_iiLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		} else {
			++each;
			printf("\tread valid lines: %d\n", (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct iiLine *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct iiLine));
			assert(temp!=NULL);
			LinesInfo=temp;
			linesNum=0;
			fill_iiLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		}
	}
	linesNum+=(each-1)*LINES_LENGTH_EACH;
	printf("\tread valid lines: %ld, file lines: %ld\n\tMax: %d, Min: %d\n", linesNum, filelineNum, maxId, minId); fflush(stdout);
	fclose(fp);

	struct iiLineFile *file=malloc(sizeof(struct iiLineFile));
	assert(file!=NULL);
	file->iMin=minId;
	file->iMax=maxId;
	file->lines=LinesInfo;
	file->linesNum=linesNum;

	return file;
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

static struct innLine fill_innLine(char *line, long filelineNum) {
	struct innLine is;

	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %ld has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", filelineNum, LINE_LENGTH_MAX-1);
		is.num = 0;
		return is;
	}

	int isMax=5000;
	char **partsLine = calloc(isMax, sizeof(void *));
	partsLine[0]=strtok(line, delimiter);
	if (partsLine[0]==NULL) {
		is.num = 0;
		printf("\tline %ld not valid, ignored (looks like a blank line).\n", filelineNum);
		return is;
	}
	int i=0;
	while((partsLine[++i]=strtok(NULL, delimiter))) {
		if (i==isMax) {
			isMax += 5000;
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

//read file to 
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

//basically, for different line styled file, I only need to change this function and struct NetLineInfo.
static void fill_i4Line(char *line, struct i4Line *LI_origin,  long *lNum, int each, int *vtMaxId, int *vtMinId, long filelineNum)
{
	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %ld has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", filelineNum, LINE_LENGTH_MAX-1);
		return;
	}

	long linesNum = *lNum+(each-1)*LINES_LENGTH_EACH;
	struct i4Line *LI = LI_origin+linesNum;

	//divide line to parts.
	//strtok return a c string(end with a '\0').
	char *partsLine[4];
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
	LI->i3=strtol(partsLine[1], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	LI->i4=strtol(partsLine[1], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}

	++(*lNum);

	//max/min Id
	if (LI->i1>LI->i2) {
		*vtMaxId=(*vtMaxId)>LI->i1?(*vtMaxId):LI->i1;
		*vtMinId=(*vtMinId)<LI->i2?(*vtMinId):LI->i2;
	} else {
		*vtMaxId=(*vtMaxId)>LI->i2?(*vtMaxId):LI->i2;
		*vtMinId=(*vtMinId)<LI->i1?(*vtMinId):LI->i1;
	}
}

//if data is stored in each line and each line contain only num & delimiter, there is no need to change this function.
struct i4LineFile *create_i4LineFile(const char * const filename)
{
	printf("read i4LineFile %s: \n", filename);
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);

	struct i4Line *LinesInfo=NULL;
	LinesInfo=malloc(LINES_LENGTH_EACH*sizeof(struct i4Line));
	assert(LinesInfo!=NULL);

	long linesNum=0;
	long filelineNum=0;
	int maxId=-1;
	int minId=INT_MAX;

	char line[LINE_LENGTH_MAX];
	int each=1;
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		++filelineNum;
		if (linesNum<LINES_LENGTH_EACH) {
			fill_i4Line(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		} else {
			++each;
			printf("\tread valid lines: %d\n", (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct i4Line *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct i4Line));
			assert(temp!=NULL);
			LinesInfo=temp;
			linesNum=0;
			fill_i4Line(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		}
	}
	linesNum+=(each-1)*LINES_LENGTH_EACH;
	printf("\tread valid lines: %ld, file lines: %ld\n\tMax: %d, Min: %d\n", linesNum, filelineNum, maxId, minId); fflush(stdout);
	fclose(fp);

	struct i4LineFile *file=malloc(sizeof(struct i4LineFile));
	assert(file!=NULL);
	file->iMin=minId;
	file->iMax=maxId;
	file->lines=LinesInfo;
	file->linesNum=linesNum;

	return file;
}


