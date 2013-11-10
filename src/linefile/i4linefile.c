#include "../../inc/linefile/i4linefile.h"
#include "../../inc/complexnet_error.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "limits.h"

//LINE_LENGTH_MAX is a serious constant, you should be sure a line's length not exceed this value.
#define LINE_LENGTH_MAX 10000

//LINES_LENGTH_EACH is the stepLength. 
//now it's 1e7, means, if a file contains less than 1e7 lines, malloc will be called only one time.
//if a file contans 1e8 lines, malloc will be called ten times.
//of course, if a file contains 1e8 lines, maybe you want to set LINES_LENGTH_EACH to 5e7 or 1e8. that's depend on you.
//you don't need to know the exactly line num of the file.
#define LINES_LENGTH_EACH 1000000

static void fill_i4Line(char *line, struct i4Line *LI_origin,  long *lNum, int each, int *vtMaxId, int *vtMinId, int *i2MaxId, int *i2MinId, long filelineNum)
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
	char *delimiter="\t, \r\n:";
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
	*vtMaxId=(*vtMaxId)>LI->i1?(*vtMaxId):LI->i1;
	*vtMinId=(*vtMinId)<LI->i1?(*vtMinId):LI->i1;
	*i2MaxId=(*i2MaxId)>LI->i2?(*i2MaxId):LI->i2;
	*i2MinId=(*i2MinId)<LI->i2?(*i2MinId):LI->i2;
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
	int i1maxId=-1;
	int i1minId=INT_MAX;
	int i2maxId = -1;
	int i2minId = INT_MAX;

	char line[LINE_LENGTH_MAX];
	int each=1;
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		++filelineNum;
		if (linesNum<LINES_LENGTH_EACH) {
			fill_i4Line(line, LinesInfo, &linesNum, each, &i1maxId, &i1minId, &i2maxId, &i2minId, filelineNum);
		} else {
			++each;
			printf("\tread valid lines: %d\n", (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct i4Line *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct i4Line));
			assert(temp!=NULL);
			LinesInfo=temp;
			linesNum=0;
			fill_i4Line(line, LinesInfo, &linesNum, each, &i1maxId, &i1minId, &i2maxId, &i2minId, filelineNum);
		}
	}
	linesNum+=(each-1)*LINES_LENGTH_EACH;
	printf("\tread valid lines: %ld, file lines: %ld\n\ti1Max: %d, i1Min: %d, i2Max: %d, i2Min: %d\n", linesNum, filelineNum, i1maxId, i1minId, i2maxId, i2minId); fflush(stdout);
	fclose(fp);

	struct i4LineFile *file=malloc(sizeof(struct i4LineFile));
	assert(file!=NULL);
	file->i1Min=i1minId;
	file->i1Max=i1maxId;
	file->i2Min=i2minId;
	file->i2Max=i2maxId;
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
void free_2_i4LineFile(struct i4LineFile *twofile) {
	if(twofile != NULL) {
		free(twofile[0].lines);
		free(twofile[1].lines);
		free(twofile);
	}
}
void print_i4LineFile(struct i4LineFile *file, char *filename) {
	long i;
	FILE *fp = fopen(filename, "w");
	fileError(fp, "print_i4LineFile");
	for (i=0; i<file->linesNum; ++i) {
		fprintf(fp, "%d::%d::%d::%d\n", file->lines[i].i1, file->lines[i].i2, file->lines[i].i3, file->lines[i].i4);
	}
	fclose(fp);
	printf("print_i4LineFile %s done. %ld lines generated.\n", filename, file->linesNum);fflush(stdout);
}
void print_2_i4LineFile(struct i4LineFile *file1, struct i4LineFile *file2, char *filename) {
	long i;
	FILE *fp = fopen(filename, "w");
	fileError(fp, "print_i4LineFile");
	for (i=0; i<file1->linesNum; ++i) {
		fprintf(fp, "%d::%d::%d::%d\n", file1->lines[i].i1, file1->lines[i].i2, file1->lines[i].i3, file1->lines[i].i4);
	}
	for (i=0; i<file2->linesNum; ++i) {
		fprintf(fp, "%d::%d::%d::%d\n", file2->lines[i].i1, file2->lines[i].i2, file2->lines[i].i3, file2->lines[i].i4);
	}
	fclose(fp);
	printf("print_2_i4LineFile %s done. %ld (%ld + %ld) lines generated.\n", filename, file1->linesNum + file2->linesNum, file1->linesNum, file2->linesNum);fflush(stdout);
}
struct i4LineFile *divide_i4LineFile(struct i4LineFile *file, double rate) {
	if (rate <=0 || rate >= 1) {
		printf("divide_i4LineFile error: wrong rate.\n");
		return NULL;
	}

	int l1, l2;
	if (file->linesNum > 100000) {
		l1 = (int)(file->linesNum*(rate+0.1));
		l2 = (int)(file->linesNum*(1-rate+0.1));
	}
	else {
		l2 = l1 = file->linesNum;
	}

	struct i4LineFile *twofile = malloc(2*sizeof(struct i4LineFile));
	assert(twofile != NULL);

	twofile[0].lines = malloc(l1*sizeof(struct i4Line));
	assert(twofile[0].lines != NULL);
	twofile[1].lines = malloc(l2*sizeof(struct i4Line));
	assert(twofile[1].lines != NULL);

	int line1=0, line2=0;
	int i1Max=-1; 
	int i2Max=-1;
    int i1Min=INT_MAX;
    int i2Min=INT_MAX;
	int _i1Max=-1; 
	int _i2Max=-1;
    int _i1Min=INT_MAX;
    int _i2Min=INT_MAX;
	long i;
	for (i=0; i<file->linesNum; ++i) {
		if (genrand_real1() < rate) {
			twofile[0].lines[line1].i1 = file->lines[i].i1;	
			twofile[0].lines[line1].i2 = file->lines[i].i2;	
			twofile[0].lines[line1].i3 = file->lines[i].i3;	
			twofile[0].lines[line1].i4 = file->lines[i].i4;	
			i1Max = i1Max>file->lines[i].i1?i1Max:file->lines[i].i1;
			i2Max = i2Max>file->lines[i].i2?i2Max:file->lines[i].i2;
			i1Min = i1Min<file->lines[i].i1?i1Min:file->lines[i].i1;
			i2Min = i2Min<file->lines[i].i2?i2Min:file->lines[i].i2;
			++line1;
		}
		else {
			twofile[1].lines[line2].i1 = file->lines[i].i1;	
			twofile[1].lines[line2].i2 = file->lines[i].i2;	
			twofile[1].lines[line2].i3 = file->lines[i].i3;	
			twofile[1].lines[line2].i4 = file->lines[i].i4;	
			_i1Max = _i1Max>file->lines[i].i1?_i1Max:file->lines[i].i1;
			_i2Max = _i2Max>file->lines[i].i2?_i2Max:file->lines[i].i2;
			_i1Min = _i1Min<file->lines[i].i1?_i1Min:file->lines[i].i1;
			_i2Min = _i2Min<file->lines[i].i2?_i2Min:file->lines[i].i2;
			++line2;
		}
	}

	if (line1>l1 || line2 >l2) {
		printf("divide_i4LineFile error: l1/l2 too small\n");
		return NULL;
	}
	twofile[0].linesNum = line1;
	twofile[0].i1Max = i1Max;
	twofile[0].i2Max = i2Max;
	twofile[0].i1Min = i1Min;
	twofile[0].i2Min = i2Min;

	twofile[1].linesNum = line2;
	twofile[1].i1Max = _i1Max;
	twofile[1].i2Max = _i2Max;
	twofile[1].i1Min = _i1Min;
	twofile[1].i2Min = _i2Min;
	printf("divide_i4LineFile done:\n\trate: %f\n\tfile1: linesNum: %d, i1Max: %d, i1Min: %d, i2Max: %d, i2Min: %d\n\tfile2: linesNum: %d, i1Max: %d, i1Min: %d, i2Max: %d, i2Min: %d\n", rate, line1, i1Max, i1Min, i2Max, i2Min, line2, _i1Max, _i1Min, _i2Max, _i2Min);fflush(stdout);
	return twofile;
}
