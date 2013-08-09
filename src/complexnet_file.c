#include "../inc/complexnet_file.h"

//basically, for different line styled file, I only need to change this function and struct LineInfo.
long fillLineInfo(char *line, int partNum, struct LineInfo *LI, long *vtMaxId, long *vtMinId)
{
	char *delimiter="\t, ";
	char *partsLine[partNum];
	int i;

	//divide line to parts.
	partsLine[0]=strtok(line, delimiter);
	if (partsLine[0]==NULL) {
		return -1;
	}
	for (i=1; i<partNum; i++) {
		partsLine[i]=strtok(NULL, delimiter);
		if (partsLine[i]==NULL) {
			return -1;
		}
	}

	//transfor parts to num(double or int);
	char *pEnd;
	LI->vt1Id=strtol(partsLine[0], &pEnd, 10);
	if (pEnd[0]!='\0') {printf("a line contain: %s, discard this line!\n", pEnd); return -1;}
	LI->vt2Id=strtol(partsLine[1], &pEnd, 10);
	if (pEnd[0]!='\0') {printf("a line contain: %s, discard this line!\n", pEnd); return -1;}
	LI->time=strtol(partsLine[2], NULL, 10);
	if (partNum==4) {
		LI->weight=strtol(partsLine[3], NULL, 10);
	}

	//max/min Id
	if (LI->vt1Id>LI->vt2Id) {
		*vtMaxId=(*vtMaxId)>LI->vt1Id?(*vtMaxId):LI->vt1Id;
		*vtMinId=(*vtMinId)<LI->vt2Id?(*vtMinId):LI->vt2Id;
	} else {
		*vtMaxId=(*vtMaxId)>LI->vt2Id?(*vtMaxId):LI->vt2Id;
		*vtMinId=(*vtMinId)<LI->vt1Id?(*vtMinId):LI->vt1Id;
	}
	return 0;
}

//if data is stored in each line and each line contain only num & delimiter, there is no need to change this function.
struct LineInfo *readFileLBL(char *filename, long *edNum, long *vtMaxId, long *vtMinId, int partNum)
{
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);

	char line[LINE_LENGTH_MAX];
	int lineNum=0;
	struct LineInfo *LinesInfo=NULL;
	LinesInfo=malloc(LINES_LENGTH_EACH*sizeof(struct LineInfo));
	memError(LinesInfo, "readFileLBL LinesInfo");
	int each=1;
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		if (lineNum<LINES_LENGTH_EACH) {
			long id=fillLineInfo(line, partNum, LinesInfo+lineNum+(each-1)*LINES_LENGTH_EACH, vtMaxId, vtMinId);
			if (id==0) ++lineNum;
		} else {
			++each;
			printf("read file %s lines: %d\r", filename, (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct LineInfo *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct LineInfo));
			memError(temp, "readFileLBL temp");
			LinesInfo=temp;
			lineNum=0;
			long id=fillLineInfo(line, partNum, LinesInfo+lineNum+(each-1)*LINES_LENGTH_EACH, vtMaxId, vtMinId);
			if (id==0) ++lineNum;
		}
	}
	*edNum=lineNum+(each-1)*LINES_LENGTH_EACH;
	printf("read file %s lines: %ld, vtMaxId: %ld, vtMinId: %ld\n", filename, *edNum, *vtMaxId, *vtMinId); fflush(stdout);
	fclose(fp);
	return LinesInfo;
}

struct LineInfo *getDaysLines(struct LineInfo *edList, int edNum, int daystart, int dayend ,int *dayNum)
{
	struct LineInfo *dayList=malloc(LINES_DAY*sizeof(struct LineInfo));
	memError(dayList, "getDaysLines dayList");
	long i, j=1, k=0;
	for (i=0; i<edNum; i++) {
		if(edList[i].time<dayend && edList[i].time>=daystart) {
			if (k==j*LINES_DAY) {
				++j;
				struct LineInfo *temp=realloc(dayList, j*LINES_DAY*sizeof(struct LineInfo));
				memError(temp, "getDaysLines temp");
				dayList=temp;
			}
			memcpy(dayList+k, edList+i, sizeof(struct LineInfo));
			++k;
		}
	}
	//printf("xx\n");
	*dayNum=k;
	return dayList;
}

struct LineInfo *AddWeight2LineInfo(struct LineInfo *dayList, int dayNum, int *realdayNum, int daystepNum)
{
	if (dayNum==0) {
		*realdayNum=0;
		return NULL;
	}
	*realdayNum=0;
	//printf("addweight2,%d.",dayNum); fflush(stdout);
	struct LineInfo *dayListWeight=calloc(dayNum, sizeof(struct LineInfo));
	//printf(",%ld,",dayListWeight); fflush(stdout);
	memError(dayListWeight, "AddWeight2LineInfo dayListWeight");
	
	int *vt1=malloc(dayNum*sizeof(int));
	memError(vt1, "AddWeight2LineInfo vt1");
	int *vt2=malloc(dayNum*sizeof(int));
	memError(vt2, "AddWeight2LineInfo vt2");
	int i;
	for (i=0; i<dayNum; i++) {
		vt1[i]=dayList[i].vt1Id;
		vt2[i]=dayList[i].vt2Id;
		dayListWeight[i].time=daystepNum;
		dayListWeight[i].weight=1;
	}
	quick_sort_int_index(vt1, 0, dayNum-1, vt2);

	int begin=0, end=0;
	for (i=0; i<dayNum; ++i) {
		if (vt1[i]==vt1[begin] && i!=dayNum-1) {
			continue;
		}
		if (vt1[i]!=vt1[begin] && i!=dayNum-1) {
			end=i;
			quick_sort_int(vt2, begin, end-1);
			moveLI2LIW(vt1, vt2, begin, end, dayListWeight, realdayNum);
			begin=i;
		}
		if (vt1[i]!=vt1[begin] && i==dayNum-1) {
			end=i;
			quick_sort_int(vt2, begin, end-1);
			moveLI2LIW(vt1, vt2, begin, end, dayListWeight, realdayNum);
			moveLI2LIW(vt1, vt2, end, end, dayListWeight, realdayNum);
		}
		if (vt1[i]==vt1[begin] && i==dayNum-1) {
			end=i+1;
			quick_sort_int(vt2, begin, end-1);
			moveLI2LIW(vt1, vt2, begin, end, dayListWeight, realdayNum);
		}
	}

	//for (i=0; i<dayNum; ++i) {
	//	printf("%d\t%d\n", vt1[i], vt2[i]);
	//}

	free(vt1);
	free(vt2);
	return dayListWeight;
}

void moveLI2LIW(int *vt1, int *vt2, int begin, int end, struct LineInfo *dayListWeight, int *realdayNum)
{
	int dl=*realdayNum;
	dayListWeight[dl].vt1Id=vt1[begin];
	dayListWeight[dl].vt2Id=vt2[begin];

	int i;
	for (i=begin+1; i<end; i++) {
		if (vt2[i]==dayListWeight[dl].vt2Id) {
			++(dayListWeight[dl].weight);
			continue;
		}
		++dl;
		dayListWeight[dl].vt1Id=vt1[i];
		dayListWeight[dl].vt2Id=vt2[i];
	}
	*realdayNum=dl+1;
}
