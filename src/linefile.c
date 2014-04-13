#include "base.h"
#include "linefile.h"
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>


#define LINES_STEP 1000000
#define LINE_LENGTH 2000
#define LINES_READIN 10000
#define ILIMIT 9
#define DLIMIT 9

static struct LineFile *init_LineFile(void) {
	struct LineFile *lf = malloc(sizeof(struct LineFile));
	if (lf == NULL) {
		return NULL;
	}

	lf->linesNum = 0;
	lf->memNum = 0;
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
	return lf;
}

static void resize_LineFile(struct LineFile *lf) {
	int **ilist[ILIMIT] = {&(lf->i1), &(lf->i2), &(lf->i3), &(lf->i4), &(lf->i5), &(lf->i6), &(lf->i7), &(lf->i8), &(lf->i9)};
	double **dlist[DLIMIT] = {&(lf->d1), &(lf->d2), &(lf->d3), &(lf->d4), &(lf->d5), &(lf->d6), &(lf->d7), &(lf->d8), &(lf->d9)};

	int i;
	for (i=0; i<ILIMIT; ++i) {
		if (*(ilist[i]) != NULL) {
			int *tmp = realloc(*(ilist[i]), (lf->memNum + LINES_STEP)*sizeof(int));
			assert(tmp != NULL);
			*(ilist[i]) = tmp;
		}
	}
	for (i=0; i<DLIMIT; ++i) {
		if (*(dlist[i]) != NULL) {
			double *tmp = realloc(*(dlist[i]), (lf->memNum + LINES_STEP)*sizeof(double));
			assert(tmp != NULL);
			*(dlist[i]) = tmp;
		}
	}
	lf->memNum += LINES_STEP;
}

static void setmem_LineFile(struct LineFile *lf, int vn, int *typelist, int ***ilist, double ***dlist) {
	int ii = 0;
	int di = 0;

	int i;
	for (i = 0; i < vn; ++i) {
		int type = typelist[i];
		switch(type) {
			case 1:
				if (ii < ILIMIT) {
					*(ilist[ii++]) = malloc(LINES_STEP * sizeof(int));
				}
				else {
					isError("create_LineFile, set large ilimit.");
				}
				break;
			case 2:
				if (di < DLIMIT) {
					*(dlist[di++]) = malloc(LINES_STEP * sizeof(double));
				}
				else {
					isError("create_LineFile, set large dlimit.");
				}
				break;
			default:
				break;
		}
	}
	lf->memNum += LINES_STEP;
}

static void set_buffer_LineFile(FILE *fp, char *buffer, int *lread) {
	*lread = 0;
	char *line = buffer;
	while((*lread) != LINES_READIN && fgets(line, LINE_LENGTH, fp)) {
		line += LINE_LENGTH;
		++(*lread);
	}
}

static void set_allparts_LineFile(char *buffer, char **allparts, int vn, int lread) {
	int i,j;
	char *line = buffer;
	char *delimiter = "\t ,:\n";
	for (i = 0; i < lread; ++i) {
		allparts[i] =strtok(line, delimiter);
		for (j=1; j<vn; ++j) {
			allparts[i + j * LINES_READIN] = strtok(NULL, delimiter);
		}
		line += LINE_LENGTH;
	}
}

static void set_lf_LineFile(struct LineFile *lf, char **allparts, int *typelist, int ***ilist, double ***dlist, int lread, int vn) {
	int i,j;
	int IL = 0;
	int DL = 0;
	int *l;
	double *d;
	char *pend;
	for (i = 0; i < vn; ++i) {
		int type = typelist[i];
		char **p = allparts + i*LINES_READIN;
		switch(type) {
			case 1:
				l = *(ilist[IL++]);
				//printf("x%ldxx\n", lf->linesNum);fflush(stdout);
				for (j = 0; j < lread; ++j) {
					 l[j+lf->linesNum] = strtol(p[j], &pend, 10);
				}
				break;
			case 2:
				d = *(dlist[DL++]);
				for (j = 0; j < lread; ++j) {
					 d[j+lf->linesNum] = strtod(p[j], &pend);
				}
				break;

		}
	}
	lf->linesNum += lread;
}

struct LineFile *create_LineFile(const char * const filename, ...) {
	struct LineFile *lf = init_LineFile();

	FILE *fp = fopen(filename, "r");
	fileError(fp, "create_LineFile");

	int **ilist[ILIMIT] = {&(lf->i1), &(lf->i2), &(lf->i3), &(lf->i4), &(lf->i5), &(lf->i6), &(lf->i7), &(lf->i8), &(lf->i9)};
	double **dlist[DLIMIT] = {&(lf->d1), &(lf->d2), &(lf->d3), &(lf->d4), &(lf->d5), &(lf->d6), &(lf->d7), &(lf->d8), &(lf->d9)};


	int typelist[ILIMIT + DLIMIT];
	va_list vl;
	va_start(vl, filename);
	int vn = 0;
	int type;
	while ((type = va_arg(vl, int))>0) {
		typelist[vn++] = type;
		printf("%d\t", type);
	}
	printf("\n");
	va_end(vl);

	setmem_LineFile(lf, vn, typelist, ilist, dlist);


	char *buffer = malloc(LINE_LENGTH * LINES_READIN * sizeof(char));
	assert(buffer != NULL);
	char **allparts = malloc(vn * LINES_READIN * sizeof(void *));
	assert(allparts != NULL);

	int lread = LINES_READIN;
	while (lread == LINES_READIN) {
		set_buffer_LineFile(fp, buffer, &lread);
		while (lf->linesNum + lread > lf->memNum) {
			resize_LineFile(lf);
		}
		//printf("lread: %d\n", lread);
		set_allparts_LineFile(buffer, allparts, vn, lread);
		set_lf_LineFile(lf, allparts, typelist, ilist, dlist, lread, vn);
	}

	return lf;
}

void print_LineFile(struct LineFile *lf, char *filename) {
	int **ilist[ILIMIT] = {&(lf->i1), &(lf->i2), &(lf->i3), &(lf->i4), &(lf->i5), &(lf->i6), &(lf->i7), &(lf->i8), &(lf->i9)};
	double **dlist[DLIMIT] = {&(lf->d1), &(lf->d2), &(lf->d3), &(lf->d4), &(lf->d5), &(lf->d6), &(lf->d7), &(lf->d8), &(lf->d9)};

	FILE *fp = fopen(filename, "w");
	fileError(fp, "print_LineFile");

	int i;
	long j;
	for (j = 0; j < lf->linesNum; ++j) {
		for (i=0; i<ILIMIT; ++i) {
			if (*(ilist[i]) != NULL) {
				fprintf(fp, "%d\t", (*(ilist[i]))[j]);
			}
		}
		for (i=0; i<DLIMIT; ++i) {
			if (*(dlist[i]) != NULL) {
				fprintf(fp, "%f\t", (*(dlist[i]))[j]);
			}
		}
		fprintf(fp, "\n");
	}
}
