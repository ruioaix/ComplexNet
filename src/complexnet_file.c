#include "../inc/complexnet_file.h"
#include "../inc/complexnet_error.h" // for fileError, memError, isError
#include <string.h> // for strtol, strtok
#include <stdio.h> // for FILE fpos_t 
#include <stdlib.h> // for malloc, remalloc
#include <assert.h> // for assert.
#include <limits.h> // for INT_MAX


static char *delimiter="\t, \r\n";
// int  =============================================================================================================================================================
static void fill_iLine(char *line, struct iLine *LI_origin,  long *lNum, int each, int *vtMaxId, int *vtMinId, long filelineNum)
{
	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %ld has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", filelineNum, LINE_LENGTH_MAX-1);
		return;
	}

	long linesNum = *lNum+(each-1)*LINES_LENGTH_EACH;
	struct iLine *LI = LI_origin+linesNum;

	//divide line to parts.
	//strtok return a c string(end with a '\0').
	char *partsLine[1];
	partsLine[0]=strtok(line, delimiter);
	if (partsLine[0]==NULL) {
		printf("\tline %ld not valid, ignored (looks like a blank line).\n", filelineNum);
		return;
	}

	//transfor parts to num(double or int);
	char *pEnd;
	LI->i1=strtol(partsLine[0], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}

	++(*lNum);

	//max/min Id
	*vtMaxId=(*vtMaxId)>LI->i1?(*vtMaxId):LI->i1;
	*vtMinId=(*vtMinId)<LI->i1?(*vtMinId):LI->i1;
}
struct iLineFile *create_iLineFile(const char * const filename) {
	printf("read iLineFile %s: \n", filename);
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);

	struct iLine *LinesInfo=NULL;
	LinesInfo=malloc(LINES_LENGTH_EACH*sizeof(struct iLine));
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
			fill_iLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		} else {
			++each;
			printf("\tread valid lines: %d\n", (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct iLine *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct iLine));
			assert(temp!=NULL);
			LinesInfo=temp;
			linesNum=0;
			fill_iLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		}
	}
	linesNum+=(each-1)*LINES_LENGTH_EACH;
	printf("\tread valid lines: %ld, file lines: %ld\n\tMax: %d, Min: %d\n", linesNum, filelineNum, maxId, minId); fflush(stdout);
	fclose(fp);

	struct iLineFile *file=malloc(sizeof(struct iLineFile));
	assert(file!=NULL);
	file->iMin=minId;
	file->iMax=maxId;
	file->lines=LinesInfo;
	file->linesNum=linesNum;

	return file;
}
void free_iLineFile(struct iLineFile *file) {
	if(file != NULL) {
		free(file->lines);
		free(file);
	}
}

//int int=============================================================================================================================================================
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
void free_iiLineFile(struct iiLineFile *file) {
	if(file != NULL) {
		free(file->lines);
		free(file);
	}
}


//unlimit int=============================================================================================================================================================
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

//four int in one line=============================================================================================================================================================
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
struct i4LineFile *create_i4LineFile(const char * const filename)
{
	printf("read i4LineFile: %s \n", filename);
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
void free_i4LineFile(struct i4LineFile *file) {
	if(file != NULL) {
		free(file->lines);
		free(file);
	}
}

//int, double, int=============================================================================================================================================================
static void fill_idiLine(char *line, struct idiLine *LI_origin,  long *lNum, int each, int *vtMaxId, int *vtMinId, long filelineNum)
{
	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %ld has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", filelineNum, LINE_LENGTH_MAX-1);
		return;
	}

	long linesNum = *lNum+(each-1)*LINES_LENGTH_EACH;
	struct idiLine *LI = LI_origin+linesNum;

	//divide line to parts.
	//strtok return a c string(end with a '\0').
	char *partsLine[3];
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

	//transfor parts to num(double or int);
	char *pEnd;
	LI->i1=strtol(partsLine[0], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	LI->d2=strtod(partsLine[1], &pEnd);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	LI->i3=strtol(partsLine[2], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}

	++(*lNum);

	*vtMaxId=(*vtMaxId)>LI->i1?(*vtMaxId):LI->i1;
	*vtMinId=(*vtMinId)<LI->i1?(*vtMinId):LI->i1;
	//max/min Id
	//if (LI->i1>LI->i2) {
	//	*vtMaxId=(*vtMaxId)>LI->i1?(*vtMaxId):LI->i1;
	//	*vtMinId=(*vtMinId)<LI->i2?(*vtMinId):LI->i2;
	//} else {
	//	*vtMaxId=(*vtMaxId)>LI->i2?(*vtMaxId):LI->i2;
	//	*vtMinId=(*vtMinId)<LI->i1?(*vtMinId):LI->i1;
	//}
}
struct idiLineFile *create_idiLineFile(const char * const filename)
{
	printf("read idiLineFile %s: \n", filename);
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);

	struct idiLine *LinesInfo=NULL;
	LinesInfo=malloc(LINES_LENGTH_EACH*sizeof(struct idiLine));
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
			fill_idiLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		} else {
			++each;
			printf("\tread valid lines: %d\n", (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct idiLine *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct idiLine));
			assert(temp!=NULL);
			LinesInfo=temp;
			linesNum=0;
			fill_idiLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		}
	}
	linesNum+=(each-1)*LINES_LENGTH_EACH;
	printf("\tread valid lines: %ld, file lines: %ld\n\tMax: %d, Min: %d\n", linesNum, filelineNum, maxId, minId); fflush(stdout);
	fclose(fp);

	struct idiLineFile *file=malloc(sizeof(struct idiLineFile));
	assert(file!=NULL);
	file->iMin=minId;
	file->iMax=maxId;
	file->lines=LinesInfo;
	file->linesNum=linesNum;

	return file;
}
void free_idiLineFile(struct idiLineFile *file) {
	if(file != NULL) {
		free(file->lines);
		free(file);
	}
}

//five int, one string, one double=============================================================================================================================================================
static void fill_i5sdLine(char *line, struct i5sdLine *LI_origin,  long *lNum, int each, int *vtMaxId, int *vtMinId, long filelineNum)
{
	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %ld has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", filelineNum, LINE_LENGTH_MAX-1);
		return;
	}

	long linesNum = *lNum+(each-1)*LINES_LENGTH_EACH;
	struct i5sdLine *LI = LI_origin+linesNum;

	//divide line to parts.
	//strtok return a c string(end with a '\0').
	char *delimiter_i5sd="\t,/\r\n";
	char *partsLine[7];
	partsLine[0]=strtok(line, delimiter_i5sd);
	if (partsLine[0]==NULL) {
		printf("\tline %ld not valid, ignored (looks like a blank line).\n", filelineNum);
		return;
	}
	partsLine[1]=strtok(NULL, delimiter_i5sd);
	if (partsLine[1]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only one number)\n", filelineNum);
		return;
	}
	partsLine[2]=strtok(NULL, delimiter_i5sd);
	if (partsLine[2]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only two number)\n", filelineNum);
		return;
	}
	partsLine[3]=strtok(NULL, delimiter_i5sd);
	if (partsLine[3]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only three number)\n", filelineNum);
		return;
	}
	partsLine[4]=strtok(NULL, delimiter_i5sd);
	if (partsLine[4]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only four number)\n", filelineNum);
		return;
	}
	partsLine[5]=strtok(NULL, delimiter_i5sd);
	if (partsLine[5]==NULL) {
		printf("\tline %ld not valid, ignored (looks like no string contain)\n", filelineNum);
		return;
	}
	partsLine[6]=strtok(NULL, delimiter_i5sd);
	if (partsLine[6]==NULL) {
		printf("\tline %ld not valid, ignored (looks like miss the last double number)\n", filelineNum);
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
	strncpy(LI->s6, partsLine[5], sizeof(LI->s6));
	LI->d7=strtod(partsLine[6], &pEnd);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}

	++(*lNum);

	*vtMaxId=(*vtMaxId)>LI->i1?(*vtMaxId):LI->i1;
	*vtMinId=(*vtMinId)<LI->i1?(*vtMinId):LI->i1;
	//max/min Id
	//if (LI->i1>LI->i2) {
	//	*vtMaxId=(*vtMaxId)>LI->i1?(*vtMaxId):LI->i1;
	//	*vtMinId=(*vtMinId)<LI->i2?(*vtMinId):LI->i2;
	//} else {
	//	*vtMaxId=(*vtMaxId)>LI->i2?(*vtMaxId):LI->i2;
	//	*vtMinId=(*vtMinId)<LI->i1?(*vtMinId):LI->i1;
	//}
}
struct i5sdLineFile *create_i5sdLineFile(const char * const filename)
{
	printf("read i5sdLineFile %s: \n", filename);
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);

	struct i5sdLine *LinesInfo=NULL;
	LinesInfo=malloc(LINES_LENGTH_EACH*sizeof(struct i5sdLine));
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
			fill_i5sdLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		} else {
			++each;
			printf("\tread valid lines: %d\n", (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct i5sdLine *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct i5sdLine));
			assert(temp!=NULL);
			LinesInfo=temp;
			linesNum=0;
			fill_i5sdLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		}
	}
	linesNum+=(each-1)*LINES_LENGTH_EACH;
	printf("\tread valid lines: %ld, file lines: %ld\n\tMax: %d, Min: %d\n", linesNum, filelineNum, maxId, minId); fflush(stdout);
	fclose(fp);

	struct i5sdLineFile *file=malloc(sizeof(struct i5sdLineFile));
	assert(file!=NULL);
	file->iMin=minId;
	file->iMax=maxId;
	file->lines=LinesInfo;
	file->linesNum=linesNum;

	return file;
}
void free_i5sdLineFile(struct i5sdLineFile *file) {
	if(file != NULL) {
		free(file->lines);
		free(file);
	}
}

//int int double int=============================================================================================================================================================
static void fill_iidiLine(char *line, struct iidiLine *LI_origin,  long *lNum, int each, int *vtMaxId, int *vtMinId, long filelineNum)
{
	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %ld has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", filelineNum, LINE_LENGTH_MAX-1);
		return;
	}

	long linesNum = *lNum+(each-1)*LINES_LENGTH_EACH;
	struct iidiLine *LI = LI_origin+linesNum;

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
	LI->d3=strtod(partsLine[2], &pEnd);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	LI->i4=strtol(partsLine[3], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}

	++(*lNum);

	*vtMaxId=(*vtMaxId)>LI->i2?(*vtMaxId):LI->i2;
	*vtMinId=(*vtMinId)<LI->i2?(*vtMinId):LI->i2;
	//max/min Id
	//if (LI->i1>LI->i2) {
	//	*vtMaxId=(*vtMaxId)>LI->i1?(*vtMaxId):LI->i1;
	//	*vtMinId=(*vtMinId)<LI->i2?(*vtMinId):LI->i2;
	//} else {
	//	*vtMaxId=(*vtMaxId)>LI->i2?(*vtMaxId):LI->i2;
	//	*vtMinId=(*vtMinId)<LI->i1?(*vtMinId):LI->i1;
	//}
}
struct iidiLineFile *create_iidiLineFile(const char * const filename)
{
	printf("read iidiLineFile %s: \n", filename);
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);

	struct iidiLine *LinesInfo=NULL;
	LinesInfo=malloc(LINES_LENGTH_EACH*sizeof(struct iidiLine));
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
			fill_iidiLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		} else {
			++each;
			printf("\tread valid lines: %d\n", (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct iidiLine *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct iidiLine));
			assert(temp!=NULL);
			LinesInfo=temp;
			linesNum=0;
			fill_iidiLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		}
	}
	linesNum+=(each-1)*LINES_LENGTH_EACH;
	printf("\tread valid lines: %ld, file lines: %ld\n\tMax: %d, Min: %d\n", linesNum, filelineNum, maxId, minId); fflush(stdout);
	fclose(fp);

	struct iidiLineFile *file=malloc(sizeof(struct iidiLineFile));
	assert(file!=NULL);
	file->iMin=minId;
	file->iMax=maxId;
	file->lines=LinesInfo;
	file->linesNum=linesNum;

	return file;
}
void free_iidiLineFile(struct iidiLineFile *file) {
	if(file != NULL) {
		free(file->lines);
		free(file);
	}
}

//five int, two string=============================================================================================================================================================
static void fill_i5ssLine(char *line, struct i5ssLine *LI_origin,  long *lNum, int each, int *vtMaxId, int *vtMinId, long filelineNum)
{
	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %ld has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", filelineNum, LINE_LENGTH_MAX-1);
		return;
	}

	long linesNum = *lNum+(each-1)*LINES_LENGTH_EACH;
	struct i5ssLine *LI = LI_origin+linesNum;

	//divide line to parts.
	//strtok return a c string(end with a '\0').
	char *delimiter_i5ss="\t/\r\n";
	char *partsLine[7];
	partsLine[0]=strtok(line, delimiter_i5ss);
	if (partsLine[0]==NULL) {
		printf("\tline %ld not valid, ignored (looks like a blank line).\n", filelineNum);
		return;
	}
	partsLine[1]=strtok(NULL, delimiter_i5ss);
	if (partsLine[1]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only one number)\n", filelineNum);
		return;
	}
	partsLine[2]=strtok(NULL, delimiter_i5ss);
	if (partsLine[2]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only two number)\n", filelineNum);
		return;
	}
	partsLine[3]=strtok(NULL, delimiter_i5ss);
	if (partsLine[3]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only three number)\n", filelineNum);
		return;
	}
	partsLine[4]=strtok(NULL, delimiter_i5ss);
	if (partsLine[4]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only four number)\n", filelineNum);
		return;
	}
	partsLine[5]=strtok(NULL, delimiter_i5ss);
	if (partsLine[5]==NULL) {
		printf("\tline %ld not valid, ignored (looks like no string)\n", filelineNum);
		return;
	}
	partsLine[6]=strtok(NULL, delimiter_i5ss);
	if (partsLine[6]==NULL) {
		printf("\tline %ld not valid, ignored (looks like miss the last string)\n", filelineNum);
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
	strncpy(LI->s6, partsLine[5], sizeof(LI->s6));
	strncpy(LI->s7, partsLine[6], sizeof(LI->s7));

	++(*lNum);

	*vtMaxId=(*vtMaxId)>LI->i1?(*vtMaxId):LI->i1;
	*vtMinId=(*vtMinId)<LI->i1?(*vtMinId):LI->i1;
	//max/min Id
	//if (LI->i1>LI->i2) {
	//	*vtMaxId=(*vtMaxId)>LI->i1?(*vtMaxId):LI->i1;
	//	*vtMinId=(*vtMinId)<LI->i2?(*vtMinId):LI->i2;
	//} else {
	//	*vtMaxId=(*vtMaxId)>LI->i2?(*vtMaxId):LI->i2;
	//	*vtMinId=(*vtMinId)<LI->i1?(*vtMinId):LI->i1;
	//}
}
struct i5ssLineFile *create_i5ssLineFile(const char * const filename)
{
	printf("read i5ssLineFile %s: \n", filename);
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);

	struct i5ssLine *LinesInfo=NULL;
	LinesInfo=malloc(LINES_LENGTH_EACH*sizeof(struct i5ssLine));
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
			fill_i5ssLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		} else {
			++each;
			printf("\tread valid lines: %d\n", (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct i5ssLine *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct i5ssLine));
			assert(temp!=NULL);
			LinesInfo=temp;
			linesNum=0;
			fill_i5ssLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		}
	}
	linesNum+=(each-1)*LINES_LENGTH_EACH;
	printf("\tread valid lines: %ld, file lines: %ld\n\tMax: %d, Min: %d\n", linesNum, filelineNum, maxId, minId); fflush(stdout);
	fclose(fp);

	struct i5ssLineFile *file=malloc(sizeof(struct i5ssLineFile));
	assert(file!=NULL);
	file->iMin=minId;
	file->iMax=maxId;
	file->lines=LinesInfo;
	file->linesNum=linesNum;

	return file;
}
void free_i5ssLineFile(struct i5ssLineFile *file) {
	if(file != NULL) {
		free(file->lines);
		free(file);
	}
}

//seven int, two double, one string=============================================================================================================================================================
static void fill_i7ddsLine(char *line, struct i7ddsLine *LI_origin,  long *lNum, int each, int *vtMaxId, int *vtMinId, long filelineNum)
{
	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %ld has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", filelineNum, LINE_LENGTH_MAX-1);
		return;
	}

	long linesNum = *lNum+(each-1)*LINES_LENGTH_EACH;
	struct i7ddsLine *LI = LI_origin+linesNum;

	//divide line to parts.
	//strtok return a c string(end with a '\0').
	char *delimiter_i7d2s="\t,/\r\n";
	char *partsLine[10];
	partsLine[0]=strtok(line, delimiter_i7d2s);
	if (partsLine[0]==NULL) {
		printf("\tline %ld not valid, ignored (looks like a blank line).\n", filelineNum);
		return;
	}
	partsLine[1]=strtok(NULL, delimiter_i7d2s);
	if (partsLine[1]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only one number)\n", filelineNum);
		return;
	}
	partsLine[2]=strtok(NULL, delimiter_i7d2s);
	if (partsLine[2]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only two number)\n", filelineNum);
		return;
	}
	partsLine[3]=strtok(NULL, delimiter_i7d2s);
	if (partsLine[3]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only three number)\n", filelineNum);
		return;
	}
	partsLine[4]=strtok(NULL, delimiter_i7d2s);
	if (partsLine[4]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only four number)\n", filelineNum);
		return;
	}
	partsLine[5]=strtok(NULL, delimiter_i7d2s);
	if (partsLine[5]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only five number)\n", filelineNum);
		return;
	}
	partsLine[6]=strtok(NULL, delimiter_i7d2s);
	if (partsLine[6]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only six number)\n", filelineNum);
		return;
	}
	partsLine[7]=strtok(NULL, delimiter_i7d2s);
	if (partsLine[7]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only seven number)\n", filelineNum);
		return;
	}
	partsLine[8]=strtok(NULL, delimiter_i7d2s);
	if (partsLine[8]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only eight number)\n", filelineNum);
		return;
	}
	partsLine[9]=strtok(NULL, delimiter_i7d2s);
	if (partsLine[9]==NULL) {
		printf("\tline %ld not valid, ignored (looks like miss the last string)\n", filelineNum);
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
	LI->i6=strtol(partsLine[5], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	LI->i7=strtol(partsLine[6], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	LI->d8=strtod(partsLine[7], &pEnd);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	LI->d9=strtod(partsLine[8], &pEnd);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	strncpy(LI->s10, partsLine[9], sizeof(LI->s10));

	++(*lNum);

	*vtMaxId=(*vtMaxId)>LI->i2?(*vtMaxId):LI->i2;
	*vtMinId=(*vtMinId)<LI->i2?(*vtMinId):LI->i2;
	//max/min Id
	//if (LI->i1>LI->i2) {
	//	*vtMaxId=(*vtMaxId)>LI->i1?(*vtMaxId):LI->i1;
	//	*vtMinId=(*vtMinId)<LI->i2?(*vtMinId):LI->i2;
	//} else {
	//	*vtMaxId=(*vtMaxId)>LI->i2?(*vtMaxId):LI->i2;
	//	*vtMinId=(*vtMinId)<LI->i1?(*vtMinId):LI->i1;
	//}
}
struct i7ddsLineFile *create_i7ddsLineFile(const char * const filename)
{
	printf("read i7ddsLineFile %s: \n", filename);
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);

	struct i7ddsLine *LinesInfo=NULL;
	LinesInfo=malloc(LINES_LENGTH_EACH*sizeof(struct i7ddsLine));
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
			fill_i7ddsLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		} else {
			++each;
			printf("\tread valid lines: %d\n", (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct i7ddsLine *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct i7ddsLine));
			assert(temp!=NULL);
			LinesInfo=temp;
			linesNum=0;
			fill_i7ddsLine(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		}
	}
	linesNum+=(each-1)*LINES_LENGTH_EACH;
	printf("\tread valid lines: %ld, file lines: %ld\n\tMax: %d, Min: %d\n", linesNum, filelineNum, maxId, minId); fflush(stdout);
	fclose(fp);

	struct i7ddsLineFile *file=malloc(sizeof(struct i7ddsLineFile));
	assert(file!=NULL);
	file->iMin=minId;
	file->iMax=maxId;
	file->lines=LinesInfo;
	file->linesNum=linesNum;

	return file;
}
void free_i7ddsLineFile(struct i7ddsLineFile *file) {
	if(file != NULL) {
		free(file->lines);
		free(file);
	}
}

//int,int, three double===================================================================================================================================================
static void fill_iid3Line(char *line, struct iid3Line *LI_origin,  long *lNum, int each, int *vtMaxId, int *vtMinId, long filelineNum)
{
	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %ld has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", filelineNum, LINE_LENGTH_MAX-1);
		return;
	}

	long linesNum = *lNum+(each-1)*LINES_LENGTH_EACH;
	struct iid3Line *LI = LI_origin+linesNum;

	//divide line to parts.
	//strtok return a c string(end with a '\0').
	char *partsLine[10];
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
	LI->d3=strtod(partsLine[2], &pEnd);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	LI->d4=strtod(partsLine[3], &pEnd);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", filelineNum, pEnd[0]);
		return;
	}
	LI->d5=strtod(partsLine[4], &pEnd);
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
struct iid3LineFile *create_iid3LineFile(const char * const filename)
{
	printf("read iid3LineFile %s: \n", filename);
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);

	struct iid3Line *LinesInfo=NULL;
	LinesInfo=malloc(LINES_LENGTH_EACH*sizeof(struct iid3Line));
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
			fill_iid3Line(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		} else {
			++each;
			printf("\tread valid lines: %d\n", (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct iid3Line *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct iid3Line));
			assert(temp!=NULL);
			LinesInfo=temp;
			linesNum=0;
			fill_iid3Line(line, LinesInfo, &linesNum, each, &maxId, &minId, filelineNum);
		}
	}
	linesNum+=(each-1)*LINES_LENGTH_EACH;
	printf("\tread valid lines: %ld, file lines: %ld\n\tMax: %d, Min: %d\n", linesNum, filelineNum, maxId, minId); fflush(stdout);
	fclose(fp);

	struct iid3LineFile *file=malloc(sizeof(struct iid3LineFile));
	assert(file!=NULL);
	file->iMin=minId;
	file->iMax=maxId;
	file->lines=LinesInfo;
	file->linesNum=linesNum;

	return file;
}
void free_iid3LineFile(struct iid3LineFile *file) {
	if(file != NULL) {
		free(file->lines);
		free(file);
	}
}
