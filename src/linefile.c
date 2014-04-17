#include "base.h"
#include "linefile.h"
#include "sort.h"
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>


#define LINES_STEP 1000000
#define LINE_LENGTH 2000
#define LINES_READIN 1000
#define ILIMIT 9
#define DLIMIT 9

//this function need to be changed when ix or dx or xxx varied.
void free_LineFile(struct LineFile *lf) {
	//TODO
	free(lf->i1);
	free(lf->i2);
	free(lf->i3);
	free(lf->i4);
	free(lf->i5);
	free(lf->i6);
	free(lf->i7);
	free(lf->i8);
	free(lf->i9);
	free(lf->d1);
	free(lf->d2);
	free(lf->d3);
	free(lf->d4);
	free(lf->d5);
	free(lf->d6);
	free(lf->d7);
	free(lf->d8);
	free(lf->d9);
	free(lf);
}

//this function need to be changed when ix or dx or xxx varied.
struct LineFile *init_LineFile(void) {
	struct LineFile *lf = malloc(sizeof(struct LineFile));
	assert(lf != NULL);
	lf->linesNum = 0;
	lf->memNum = 0;
	lf->filename = NULL;
	//TODO
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

//this function need to be changed when ix or dx or xxx varied.
static void set_ilist_LineFile(int ***ilist, struct LineFile *lf) {
	ilist[0] = &(lf->i1);
	ilist[1] = &(lf->i2);
	ilist[2] = &(lf->i3);
	ilist[3] = &(lf->i4);
	ilist[4] = &(lf->i5);
	ilist[5] = &(lf->i6);
	ilist[6] = &(lf->i7);
	ilist[7] = &(lf->i8);
	ilist[8] = &(lf->i9);
}
static void set_dlist_LineFile(double ***dlist, struct LineFile *lf) {
	dlist[0] = &(lf->d1);
	dlist[1] = &(lf->d2);
	dlist[2] = &(lf->d3);
	dlist[3] = &(lf->d4);
	dlist[4] = &(lf->d5);
	dlist[5] = &(lf->d6);
	dlist[6] = &(lf->d7);
	dlist[7] = &(lf->d8);
	dlist[8] = &(lf->d9);
}
static void add_memory_LineFile(struct LineFile *lf, int ***ilist, double ***dlist) {
	int i;
	for (i=0; i<ILIMIT; ++i) {
		if (*(ilist[i]) != NULL) {
			int *tmp = realloc(*(ilist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(int));
			assert(tmp != NULL);
			*(ilist[i]) = tmp;
		}
	}
	for (i=0; i<DLIMIT; ++i) {
		if (*(dlist[i]) != NULL) {
			double *tmp = realloc(*(dlist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(double));
			assert(tmp != NULL);
			*(dlist[i]) = tmp;
		}
	}
	lf->memNum += LINES_STEP;
}
static void init_memory_LineFile(struct LineFile *lf, int vn, int *typelist, int ***ilist, double ***dlist) {
	int ii = 0;
	int di = 0;
	int i;
	for (i = 0; i < vn; ++i) {
		int type = typelist[i];
		switch(type) {
			case 1:
				if (ii < ILIMIT) {
					int *tmp = malloc(LINES_STEP * sizeof(int));
					assert(tmp != NULL);
					*(ilist[ii++]) = tmp;
				}
				else {
					isError("create_LineFile, set large ilimit.");
				}
				break;
			case 2:
				if (di < DLIMIT) {
					double *tmp = malloc(LINES_STEP * sizeof(double));
					assert(tmp != NULL);
					*(dlist[di++]) = tmp;
				}
				else {
					isError("create_LineFile, set large dlimit.");
				}
				break;
			default:
				isError("wrong type in init_memory_LineFile");
				break;
		}
	}
	lf->memNum += LINES_STEP;
}
static void set_buffer_LineFile(FILE *fp, char *buffer, int *lread) {
	char *line = buffer;
	*lread = 0;
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
static void set_lf_LineFile(struct LineFile *lf, char **allparts, int *typelist, int ***ilist, double ***dlist, int lread, int vn, char *isok) {
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
				for (j = 0; j < lread; ++j) {
					if (p[j] != NULL) {
						l[j+lf->linesNum] = strtol(p[j], &pend, 10);
						if (pend[0]!='\0') {
							printf("create LineFile =>> %s file, line: %ld, i%d part.\n", lf->filename, j+lf->linesNum, IL);
							*isok = 0;
						}
					}
					else {
						l[j+lf->linesNum] = -1;
					}
				}
				break;
			case 2:
				d = *(dlist[DL++]);
				for (j = 0; j < lread; ++j) {
					if (p[j] != NULL) {
						d[j+lf->linesNum] = strtod(p[j], &pend);
						if (pend[0]!='\0') {
							printf("create LineFile =>> %s file, line: %ld, d%d part.\n", lf->filename, j+lf->linesNum, IL);
							*isok = 0;
						}
					}
					else {
						d[j+lf->linesNum] = -1;
					}
				}
				break;
			default:
				isError("wrong type in set_lf_LineFile");
		}
	}
	lf->linesNum += lread;
}

struct LineFile *create_LineFile(char *filename, ...) {
	//check filename.
	FILE *fp = fopen(filename, "r");
	fileError(fp, "create_LineFile");

	//the return lf.
	struct LineFile *lf = init_LineFile();
	lf->filename = filename;

	//get ilist and dlist.
	int **ilist[ILIMIT];
	double **dlist[DLIMIT];
	set_ilist_LineFile(ilist, lf);
	set_dlist_LineFile(dlist, lf);

	//get typelist.
	int typelist[ILIMIT + DLIMIT];
	va_list vl;
	va_start(vl, filename);
	int vn = 0, type;
	while ((type = va_arg(vl, int))>0) {
		typelist[vn++] = type;
	}
	va_end(vl);

	//set lf memory with typelist.
	init_memory_LineFile(lf, vn, typelist, ilist, dlist);

	//buffer used to read file.
	char isok = 1;
	char *buffer = malloc(LINE_LENGTH * LINES_READIN * sizeof(char));
	assert(buffer != NULL);
	char **allparts = malloc(vn * LINES_READIN * sizeof(void *));
	assert(allparts != NULL);
	int lread = LINES_READIN;
	while (lread == LINES_READIN) {
		set_buffer_LineFile(fp, buffer, &lread);
		while (lf->linesNum + lread > lf->memNum) {
			add_memory_LineFile(lf, ilist, dlist);
		}
		set_allparts_LineFile(buffer, allparts, vn, lread);
		set_lf_LineFile(lf, allparts, typelist, ilist, dlist, lread, vn, &isok);
	}
	free(buffer);
	free(allparts);
	fclose(fp);

	if (!isok) {
		printf("create Linefile =>> %s has some non-valid lines, program stop.\n", lf->filename);
		exit(-1);
	}

	printf("create Linefile =>> read %s successfully.\n", lf->filename);
	return lf;
}

void print_LineFile(struct LineFile *lf, char *filename) {
	FILE *fp = fopen(filename, "w");
	fileError(fp, "print_LineFile");
	int **ilist[ILIMIT];
	double **dlist[DLIMIT];
	set_ilist_LineFile(ilist, lf);
	set_dlist_LineFile(dlist, lf);
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
	fclose(fp);
}

struct LineFile *add_LineFile(struct LineFile *lf1, struct LineFile *lf2) {
	if (lf1 == NULL || lf2 == NULL || lf1->linesNum < 1 || lf2->linesNum < 1) isError("add_LineFile");

	struct LineFile *lf = init_LineFile();

	int **ilist[ILIMIT];
	double **dlist[DLIMIT];
	set_ilist_LineFile(ilist, lf);
	set_dlist_LineFile(dlist, lf);

	int **ilist1[ILIMIT];
	double **dlist1[DLIMIT];
	set_ilist_LineFile(ilist1, lf1);
	set_dlist_LineFile(dlist1, lf1);

	int **ilist2[ILIMIT];
	double **dlist2[DLIMIT];
	set_ilist_LineFile(ilist2, lf2);
	set_dlist_LineFile(dlist2, lf2);

	lf->linesNum = lf1->linesNum + lf2->linesNum;

	int i;
	long j;
	for (i=0; i<ILIMIT; ++i) {
		if (*(ilist1[i]) != NULL && *(ilist2[i]) != NULL) {
			*(ilist[i]) = malloc((lf->linesNum)*sizeof(int));
			assert(*(ilist[i]) != NULL);
			for (j = 0; j < lf1->linesNum; ++j) {
				(*(ilist[i]))[j] = (*(ilist1[i]))[j];
			}
			for (j = 0; j < lf2->linesNum; ++j) {
				(*(ilist[i]))[j+lf1->linesNum] = (*(ilist2[i]))[j];
			}
		}
		else if (*(ilist1[i]) == NULL && *(ilist2[i]) == NULL) {
			//*(ilist[i]) == NULL;
		}
		else {
			isError("lf1 and lf2 have different Structures, can not add lf1 with lf2.");
		}
	}
	for (i=0; i<DLIMIT; ++i) {
		if (*(dlist1[i]) != NULL && *(dlist2[i]) != NULL) {
			*(dlist[i]) = malloc((lf1->linesNum + lf2->linesNum)*sizeof(double));
			assert(*(dlist[i]) != NULL);
			for (j = 0; j < lf1->linesNum; ++j) {
				(*(dlist[i]))[j] = (*(dlist1[i]))[j];
			}
			for (j = 0; j < lf2->linesNum; ++j) {
				(*(dlist[i]))[j+lf1->linesNum] = (*(dlist2[i]))[j];
			}
		}
		else if (*(dlist1[i]) == NULL && *(dlist2[i]) == NULL) {
			//*(dlist[i]) == NULL;
		}
		else {
			isError("lf1 and lf2 have different Structures, can not add lf1 with lf2.");
		}
	}

	lf->memNum = lf->linesNum;
	lf->filename = "addlinefile";
	return lf;
}
