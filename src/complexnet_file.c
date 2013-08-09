#include "../inc/complexnet_file.h"

//basically, for different line styled file, I only need to change this function and struct LineInfo.
long fillLineInfo(char *line, struct LineInfo *LI, long *vtMaxId, long *vtMinId)
{

	//divide line to parts.
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
	assert(pEnd[1]=='\0');

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
struct LineInfo *readFileLBL(char *filename, long *edNum, long *vtMaxId, long *vtMinId)
{
	//open file
	FILE *fp=fopen(filename,"r");
	fileError(fp, filename);

	char line[LINE_LENGTH_MAX];
	int lineNum=0;
	struct LineInfo *LinesInfo=NULL;
	LinesInfo=malloc(LINES_LENGTH_EACH*sizeof(struct LineInfo));
	assert(LinesInfo!=NULL);
	int each=1;
	while(fgets(line, LINE_LENGTH_MAX, fp)) {
		if (lineNum<LINES_LENGTH_EACH) {
			long id=fillLineInfo(line, LinesInfo+lineNum+(each-1)*LINES_LENGTH_EACH, vtMaxId, vtMinId);
			if (id==0) ++lineNum;
		} else {
			++each;
			printf("read file %s lines: %d\r", filename, (each-1)*LINES_LENGTH_EACH); fflush(stdout);
			struct LineInfo *temp=realloc(LinesInfo, each*LINES_LENGTH_EACH*sizeof(struct LineInfo));
			assert(temp!=NULL);
			LinesInfo=temp;
			lineNum=0;
			long id=fillLineInfo(line, LinesInfo+lineNum+(each-1)*LINES_LENGTH_EACH, vtMaxId, vtMinId);
			if (id==0) ++lineNum;
		}
	}
	*edNum=lineNum+(each-1)*LINES_LENGTH_EACH;
	printf("read file %s lines: %ld, vtMaxId: %ld, vtMinId: %ld\n", filename, *edNum, *vtMaxId, *vtMinId); fflush(stdout);
	fclose(fp);
	return LinesInfo;
}
