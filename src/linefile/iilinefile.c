static char *delimiter="\t, \r\n:";
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
