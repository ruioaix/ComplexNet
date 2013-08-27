#include "../inc/complexnet_file.h"
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
#include "../inc/complexnet_error.h"


static char *delimiter="\t, \r\n";

void free_iiLineFile(struct iiLineFile *file) {
	if(file != NULL) {
		free(file->lines);
		free(file);
	}
}

//basically, for different line styled file, I only need to change this function and struct NetLineInfo.
static void fill_iiLine(char *line, struct iiLine *LI_origin,  long *lNum, int each, int *vtMaxId, int *vtMinId)
{

	static long fill_iiLine_call_count = 0;
	++fill_iiLine_call_count;

	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %ld has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", fill_iiLine_call_count, LINE_LENGTH_MAX-1);
		return;
	}

	long linesNum = *lNum+(each-1)*LINES_LENGTH_EACH;
	struct iiLine *LI = LI_origin+linesNum;

	//divide line to parts.
	//strtok return a c string(end with a '\0').
	char *partsLine[2];
	partsLine[0]=strtok(line, delimiter);
	if (partsLine[0]==NULL) {
		printf("\tline %ld not valid, ignored (looks like a blank line).\n", fill_iiLine_call_count);
		return;
	}
	partsLine[1]=strtok(NULL, delimiter);
	if (partsLine[1]==NULL) {
		printf("\tline %ld not valid, ignored (looks like only one number)\n", fill_iiLine_call_count);
		return;
	}

	//transfor parts to num(double or int);
	char *pEnd;
	LI->i1=strtol(partsLine[0], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", fill_iiLine_call_count, pEnd[0]);
		return;
	}
	LI->i2=strtol(partsLine[1], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", fill_iiLine_call_count, pEnd[0]);
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
	printf("read Net file %s: \n", filename);
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
		if (linesNum<LINES_LENGTH_EACH) {
			fill_iiLine(line, LinesInfo, &linesNum, each, &maxId, &minId);
		} else {
			++each;
			printf("\tread valid lines: %d\n", (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct iiLine *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct iiLine));
			assert(temp!=NULL);
			LinesInfo=temp;
			linesNum=0;
			fill_iiLine(line, LinesInfo, &linesNum, each, &maxId, &minId);
		}
		++filelineNum;
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

static struct innLine fill_innLine(char *line) {
	static long fill_innLine_call_count = 0;
	++fill_innLine_call_count;

	struct innLine is;

	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %ld has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", fill_innLine_call_count, LINE_LENGTH_MAX-1);
		is.num = 0;
		return is;
	}

	int isMax=10000;
	char **partsLine = calloc(isMax, sizeof(void *));
	partsLine[0]=strtok(line, delimiter);
	if (partsLine[0]==NULL) {
		is.num = 0;
		printf("\tline %ld not valid, ignored (looks like a blank line).\n", fill_innLine_call_count);
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
			printf("\tline %ld not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", fill_innLine_call_count, pEnd[0]);
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
	printf("read IS file %s: \n", filename);
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
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		struct innLine is = fill_innLine(line);
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

