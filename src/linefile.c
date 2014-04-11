#include "base.h"
#include "linefile.h"
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>


#define LINES_STEP 1000000
#define LINE_LENGTH 10000
#define ILIMIT 9
#define DLIMIT 9

static void init_LineFile(struct LineFile *lf) {
	lf->i1 = NULL;
	lf->i2 = NULL;
	lf->i3 = NULL;
	lf->i4 = NULL;
	lf->i5 = NULL;
	lf->i6 = NULL;
	lf->i7 = NULL;
	lf->i8 = NULL;
	lf->i9 = NULL;
	lf->d1 = NULL;
	lf->d2 = NULL;
	lf->d3 = NULL;
	lf->d4 = NULL;
	lf->d5 = NULL;
	lf->d6 = NULL;
	lf->d7 = NULL;
	lf->d8 = NULL;
	lf->d9 = NULL;
}

static void resize_LineFile(struct LineFile *lf) {
	int *ilist[ILIMIT] = {lf->i1, lf->i2, lf->i3, lf->i4, lf->i5, lf->i6, lf->i7, lf->i8, lf->i9};
	double *dlist[DLIMIT] = {lf->d1, lf->d2, lf->d3, lf->d4, lf->d5, lf->d6, lf->d7, lf->d8, lf->d9};
	int i;
	for (i=0; i<ILIMIT; ++i) {
		if (ilist[i] != NULL) {
			int *tmp = realloc(ilist[i], (lf->memNum + LINES_STEP)*sizeof(int));
			assert(tmp != NULL);
			ilist[i] = tmp;
		}
	}
	for (i=0; i<DLIMIT; ++i) {
		if (dlist[i] != NULL) {
			double *tmp = realloc(dlist[i], (lf->memNum + LINES_STEP)*sizeof(double));
			assert(tmp != NULL);
			dlist[i] = tmp;
		}
	}
	lf->memNum += LINES_STEP;
}

static void setmem_LineFile(struct LineFile *lf, int vn, int *typelist) {
	int ii = 0;
	int di = 0;
	int *ilist[ILIMIT] = {lf->i1, lf->i2, lf->i3, lf->i4, lf->i5, lf->i6, lf->i7, lf->i8, lf->i9};
	double *dlist[DLIMIT] = {lf->d1, lf->d2, lf->d3, lf->d4, lf->d5, lf->d6, lf->d7, lf->d8, lf->d9};

	int i;
	for (i = 0; i < vn; ++i) {
		int type = typelist[i];
		switch(type) {
			case 0:
				if (ii < ILIMIT) {
					ilist[ii++] = malloc(LINES_STEP * sizeof(int));
				}
				else {
					isError("create_LineFile, set large ilimit.");
				}
				break;
			case 1:
				if (di < DLIMIT) {
					dlist[di++] = malloc(LINES_STEP * sizeof(double));
				}
				else {
					isError("create_LineFile, set large dlimit.");
				}
				break;
			default:
				break;
		}
	}
}

struct LineFile *create_LineFile(const char * const filename, ...) {
	struct LineFile *lf = malloc(sizeof(struct LineFile));
	assert(lf != NULL);
	init_LineFile(lf);

	FILE *fp = fopen(filename, "r");
	fileError(fp, "create_LineFile");

	int typelist[ILIMIT + DLIMIT];
	va_list vl;
	va_start(vl, filename);
	int vn = 0;
	int type;
	while ((type = va_arg(vl, int))>0) {
		typelist[vn++] = type;
		printf("%d\t", type);
	}
	va_end(vl);
	setmem_LineFile(lf, vn, typelist);


	long memNum = LINES_STEP;
	long linesNum = 0;
	long filelinesNum = 0;
	char line[LINE_LENGTH];
	while(fgets(line, LINE_LENGTH, fp)) {
		++filelinesNum;
		if (linesNum<memNum) {
			//fill_iiLine(line, LinesInfo, &linesNum, each, &maxId, &minId, &_maxId, &_minId, filelineNum);
		} else {
			printf("\tread valid lines: %ld\n", linesNum); fflush(stdout);
			resize_LineFile(lf);
			//fill_iiLine(line, LinesInfo, &linesNum, each, &maxId, &minId, &_maxId, &_minId, filelineNum);
		}
	}

	return NULL;
}
