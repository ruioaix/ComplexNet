#include "../inc/complexnet_file.h"

//
void freeNetFile(struct NetFile *file) {
	if(file != NULL) {
		free(file->lines);
		free(file);
	}
}

//basically, for different line styled file, I only need to change this function and struct LineInfo.
void fillLineInfo(char *line, struct LineInfo *LI, vttype *vtMaxId, vttype *vtMinId)
{
	//divide line to parts.
	//strtok return a c string(end with a '\0').
	char *delimiter="\t, ";
	char *partsLine[2];
	partsLine[0]=strtok(line, delimiter);
	assert(partsLine[0]!=NULL);
	partsLine[1]=strtok(NULL, delimiter);
	assert(partsLine[1]!=NULL);

	//transfor parts to num(double or int);
	char *pEnd;
	LI->vt1Id=strtol(partsLine[0], &pEnd, 10);
	assert(pEnd[0]=='\0');
	LI->vt2Id=strtol(partsLine[1], &pEnd, 10);
	assert(pEnd[0]=='\0' || pEnd[0]=='\n' || pEnd[0]=='\r');

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
struct NetFile *readFileLBL(const char * const filename)
{
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);

	struct LineInfo *LinesInfo=NULL;
	LinesInfo=malloc(LINES_LENGTH_EACH*sizeof(struct LineInfo));
	assert(LinesInfo!=NULL);

	edtype lineNum=0;
	vttype maxId=-1;
	vttype minId=INT_MAX;

	char line[LINE_LENGTH_MAX];
	int each=1;
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		if (lineNum<LINES_LENGTH_EACH) {
			fillLineInfo(line, LinesInfo+lineNum+(each-1)*LINES_LENGTH_EACH, &maxId, &minId);
			++lineNum;
		} else {
			++each;
			printf("read file %s lines: %d\r", filename, (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct LineInfo *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct LineInfo));
			assert(temp!=NULL);
			LinesInfo=temp;
			lineNum=0;
			fillLineInfo(line, LinesInfo+lineNum+(each-1)*LINES_LENGTH_EACH, &maxId, &minId);
			++lineNum;
		}
	}
	lineNum+=(each-1)*LINES_LENGTH_EACH;
	printf("read file %s lines: %d\n\tMax: %d, Min: %d\n", filename, lineNum, maxId, minId); fflush(stdout);
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
struct InfectSourceFile *readISfromFile(const char * const filename)
{
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);
	//record file begin position
	fpos_t file_position;
	fgetpos(fp, &file_position);

	struct InfectSourceFile *isfile=malloc(sizeof(struct InfectSourceFile));
	assert(isfile!=NULL);

	char line[LINE_LENGTH_MAX];
	int line_Num=0;
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		line_Num++;
	}
	assert(line_Num!=0);

	isfile->lines = calloc(line_Num, sizeof(struct InfectSource));
	assert(isfile->lines!=NULL);

	fsetpos(fp, &file_position);
	int linesNum=0;
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		struct InfectSource is = fillISfromLine(line);
		if (is.num!=0) {
			isfile->lines[linesNum++]=is;
		}
	}
	fclose(fp);
	isfile->ISsNum=linesNum;
	assert(linesNum!=0);

	return isfile;
}

struct InfectSource fillISfromLine(char *line) {
	struct InfectSource is;
	int isMax=10000;
	char *delimiter="\t, \r\n";
	char **partsLine = calloc(isMax, sizeof(void *));
	partsLine[0]=strtok(line, delimiter);
	if (partsLine[0]==NULL) {
		is.num = 0;
		return is;
	}
	vttype i=1;
	while((partsLine[i++]=strtok(NULL, delimiter))) {
		if (i==isMax) {
			isMax += 5000;
			char **temp = realloc(partsLine, isMax*sizeof(void *));
			assert(temp!=NULL);
			partsLine=temp;
		}
	}

	vttype num=i-1;
	vttype *vt=malloc(num*sizeof(vttype));
	vttype j=0;
	char *pEnd;
	for (j=0; j<num; ++j) {
		vt[j]=strtol(partsLine[j], &pEnd, 10);
		assert(pEnd[0]=='\0');
	}

	free(partsLine);

	is.num=num;
	is.vt=vt;
	return is;
}
