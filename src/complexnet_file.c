#include "../inc/complexnet_file.h"

//
void freeNetFile(struct NetFile *file) {
	if(file != NULL) {
		free(file->lines);
		free(file);
	}
}

//basically, for different line styled file, I only need to change this function and struct NetLineInfo.
void fillNetLineInfo(char *line, struct NetLineInfo *LI_origin,  edtype *lNum, int each, vttype *vtMaxId, vttype *vtMinId)
{

	static edtype fillNetLineInfo_call_count = 0;
	++fillNetLineInfo_call_count;

	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %d has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", fillNetLineInfo_call_count, LINE_LENGTH_MAX-1);
		return;
	}

	edtype lineNum = *lNum+(each-1)*LINES_LENGTH_EACH;
	struct NetLineInfo *LI = LI_origin+lineNum;

	//divide line to parts.
	//strtok return a c string(end with a '\0').
	char *delimiter="\t, \r\n";
	char *partsLine[2];
	partsLine[0]=strtok(line, delimiter);
	if (partsLine[0]==NULL) {
		printf("\tline %d not valid, ignored (looks like a blank line).\n", fillNetLineInfo_call_count);
		return;
	}
	partsLine[1]=strtok(NULL, delimiter);
	if (partsLine[1]==NULL) {
		printf("\tline %d not valid, ignored (looks like only one number)\n", fillNetLineInfo_call_count);
		return;
	}
	//transfor parts to num(double or int);
	char *pEnd;
	LI->vt1Id=strtol(partsLine[0], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %d not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", fillNetLineInfo_call_count, pEnd[0]);
		return;
	}
	LI->vt2Id=strtol(partsLine[1], &pEnd, 10);
	if (pEnd[0]!='\0') {
		printf("\tline %d not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", fillNetLineInfo_call_count, pEnd[0]);
		return;
	}

	++(*lNum);

	//max/min Id
	if (LI->vt1Id>LI->vt2Id) {
		*vtMaxId=(*vtMaxId)>LI->vt1Id?(*vtMaxId):LI->vt1Id;
		*vtMinId=(*vtMinId)<LI->vt2Id?(*vtMinId):LI->vt2Id;
	} else {
		*vtMaxId=(*vtMaxId)>LI->vt2Id?(*vtMaxId):LI->vt2Id;
		*vtMinId=(*vtMinId)<LI->vt1Id?(*vtMinId):LI->vt1Id;
	}
}

//if data is stored in each line and each line contain only num & delimiter, there is no need to change this function.
struct NetFile *readNetFile(const char * const filename)
{
	printf("read Net file %s: \n", filename);
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);

	struct NetLineInfo *LinesInfo=NULL;
	LinesInfo=malloc(LINES_LENGTH_EACH*sizeof(struct NetLineInfo));
	assert(LinesInfo!=NULL);

	edtype lineNum=0;
	edtype filelineNum=0;
	vttype maxId=-1;
	vttype minId=INT_MAX;

	char line[LINE_LENGTH_MAX];
	int each=1;
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		if (lineNum<LINES_LENGTH_EACH) {
			fillNetLineInfo(line, LinesInfo, &lineNum, each, &maxId, &minId);
		} else {
			++each;
			printf("\tread valid lines: %d\n", (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct NetLineInfo *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct NetLineInfo));
			assert(temp!=NULL);
			LinesInfo=temp;
			lineNum=0;
			fillNetLineInfo(line, LinesInfo, &lineNum, each, &maxId, &minId);
		}
		++filelineNum;
	}
	lineNum+=(each-1)*LINES_LENGTH_EACH;
	printf("\tread valid lines: %d, file lines: %d\n\tMax: %d, Min: %d\n", lineNum, filelineNum, maxId, minId); fflush(stdout);
	fclose(fp);

	struct NetFile *file=malloc(sizeof(struct NetFile));
	assert(file!=NULL);
	file->minId=minId;
	file->maxId=maxId;
	file->lines=LinesInfo;
	file->linesNum=lineNum;

	return file;
}

void freeISFile(struct InfectSourceFile *file) {
	int i;
	if (file != NULL) {
		for(i=0; i<file->ISsNum; ++i) {
			free(file->lines[i].vt);
		}
		free(file->lines);
		free(file);
	}
}

//read file to 
struct InfectSourceFile *readAllISfromFile(const char * const filename)
{
	printf("read IS file %s: \n", filename);
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);
	//record file begin position
	fpos_t file_position;
	fgetpos(fp, &file_position);

	struct InfectSourceFile *isfile=malloc(sizeof(struct InfectSourceFile));
	assert(isfile!=NULL);

	char line[LINE_LENGTH_MAX];
	int filelines=0;
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		++filelines;
	}
	assert(filelines!=0);

	isfile->lines = calloc(filelines, sizeof(struct InfectSource));
	assert(isfile->lines!=NULL);

	fsetpos(fp, &file_position);
	int linesNum=0;
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		struct InfectSource is = fillISfromLine(line);
		if (is.num!=0) {
			is.ISId=linesNum;
			isfile->lines[linesNum++]=is;
		}
	}
	fclose(fp);
	isfile->ISsNum=linesNum;
	assert(linesNum!=0);
	printf("\ttotally %d groups of IS, file lines: %d\n", linesNum, filelines);

	return isfile;
}

struct InfectSource fillISfromLine(char *line) {
	static vttype fillISfromLine_call_count = 0;
	++fillISfromLine_call_count;

	struct InfectSource is;

	if (strlen(line) == LINE_LENGTH_MAX-1) {
		printf("\tthe line %d has %d characters, ignored, because most likely you get an incomplete line, set LINE_LENGTH_MAX larger.\n", fillISfromLine_call_count, LINE_LENGTH_MAX-1);
		is.num = 0;
		return is;
	}

	int isMax=10000;
	char *delimiter="\t, \r\n";
	char **partsLine = calloc(isMax, sizeof(void *));
	partsLine[0]=strtok(line, delimiter);
	if (partsLine[0]==NULL) {
		is.num = 0;
		printf("\tline %d not valid, ignored (looks like a blank line).\n", fillISfromLine_call_count);
		return is;
	}
	vttype i=0;
	while((partsLine[++i]=strtok(NULL, delimiter))) {
		if (i==isMax) {
			isMax += 5000;
			char **temp = realloc(partsLine, isMax*sizeof(void *));
			assert(temp!=NULL);
			partsLine=temp;
		}
	}

	vttype num=i;
	vttype *vt=malloc(num*sizeof(vttype));
	vttype j=0;
	char *pEnd;
	for (j=0; j<num; ++j) {
		vt[j]=strtol(partsLine[j], &pEnd, 10);
		if (pEnd[0]!='\0') {
			free(vt);
			is.num = 0;
			printf("\tline %d not valid, ignored (looks like contain some char which is not number, like: \"%c\").\n", fillISfromLine_call_count, pEnd[0]);
			free(partsLine);
			return is;
		}
	}

	free(partsLine);

	is.num=num;
	is.vt=vt;
	return is;
}
