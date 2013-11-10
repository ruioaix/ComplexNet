
static char *delimiter="\t, \r\n:";
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
