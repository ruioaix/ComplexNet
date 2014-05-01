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

//put i/d/c/l/ccx 's address into i/d/c/l/cclist
static void set_list_LineFile(struct LineFile *lf) {
	lf->ilist[0] = &(lf->i1);
	lf->ilist[1] = &(lf->i2);
	lf->ilist[2] = &(lf->i3);
	lf->ilist[3] = &(lf->i4);
	lf->ilist[4] = &(lf->i5);
	lf->ilist[5] = &(lf->i6);
	lf->ilist[6] = &(lf->i7);
	lf->ilist[7] = &(lf->i8);
	lf->ilist[8] = &(lf->i9);

	lf->dlist[0] = &(lf->d1);
	lf->dlist[1] = &(lf->d2);
	lf->dlist[2] = &(lf->d3);
	lf->dlist[3] = &(lf->d4);
	lf->dlist[4] = &(lf->d5);
	lf->dlist[5] = &(lf->d6);
	lf->dlist[6] = &(lf->d7);
	lf->dlist[7] = &(lf->d8);
	lf->dlist[8] = &(lf->d9);

	lf->clist[0] = &(lf->c1);
	lf->clist[1] = &(lf->c2);
	lf->clist[2] = &(lf->c3);
	lf->clist[3] = &(lf->c4);
	lf->clist[4] = &(lf->c5);
	lf->clist[5] = &(lf->c6);
	lf->clist[6] = &(lf->c7);
	lf->clist[7] = &(lf->c8);
	lf->clist[8] = &(lf->c9);

	lf->llist[0] = &(lf->l1);
	lf->llist[1] = &(lf->l2);
	lf->llist[2] = &(lf->l3);
	lf->llist[3] = &(lf->l4);
	lf->llist[4] = &(lf->l5);
	lf->llist[5] = &(lf->l6);
	lf->llist[6] = &(lf->l7);
	lf->llist[7] = &(lf->l8);
	lf->llist[8] = &(lf->l9);

	lf->cclist[0] = &(lf->cc1);
	lf->cclist[1] = &(lf->cc2);
	lf->cclist[2] = &(lf->cc3);
	lf->cclist[3] = &(lf->cc4);
	lf->cclist[4] = &(lf->cc5);
	lf->cclist[5] = &(lf->cc6);
	lf->cclist[6] = &(lf->cc7);
	lf->cclist[7] = &(lf->cc8);
	lf->cclist[8] = &(lf->cc9);
}
//create a empty but completive LineFile.
static struct LineFile *init_LineFile(void) {
	struct LineFile *lf = malloc(sizeof(struct LineFile));
	assert(lf != NULL);
	lf->linesNum = 0;
	lf->memNum = 0;
	lf->filename = "NewEmptyLF";

	lf->iNum = 9;
	lf->dNum = 9;
	lf->cNum = 9;
	lf->lNum = 9;
	lf->ccNum = 9;
	lf->ilist = malloc(lf->iNum*sizeof(void **));
	assert(lf->ilist != NULL);
	lf->dlist = malloc(lf->dNum*sizeof(void **));
	assert(lf->dlist != NULL);
	lf->clist = malloc(lf->cNum*sizeof(void **));
	assert(lf->clist != NULL);
	lf->llist = malloc(lf->lNum*sizeof(void **));
	assert(lf->llist != NULL);
	lf->cclist = malloc(lf->ccNum*sizeof(void ***));
	assert(lf->cclist != NULL);
	set_list_LineFile(lf);

	int i;
	for (i = 0; i < lf->iNum; ++i) {
		*(lf->ilist[i]) = NULL;
	}
	for (i = 0; i < lf->dNum; ++i) {
		*(lf->dlist[i]) = NULL;
	}
	for (i = 0; i < lf->cNum; ++i) {
		*(lf->clist[i]) = NULL;
	}
	for (i = 0; i < lf->lNum; ++i) {
		*(lf->llist[i]) = NULL;
	}
	for (i = 0; i < lf->ccNum; ++i) {
		*(lf->cclist[i]) = NULL;
	}
	return lf;
}
//alloc memory according to typelist.
static void init_memory_LineFile(struct LineFile *lf, int vn, int *typelist) {
	int ii = 0;
	int di = 0;
	int ci = 0;
	int li = 0;
	int cci = 0;
	int ***ilist = lf->ilist;
	double ***dlist = lf->dlist;
	char ***clist = lf->clist;
	long ***llist = lf->llist;
	char ****cclist = lf->cclist;
	int i;
	for (i = 0; i < vn; ++i) {
		int type = typelist[i];
		switch(type) {
			case 1:
				if (ii < lf->iNum) {
					int *tmp = malloc(LINES_STEP * sizeof(int));
					assert(tmp != NULL);
					*(ilist[ii++]) = tmp;
				}
				else {
					isError("create_LineFile, set large ilimit.");
				}
				break;
			case 2:
				if (di < lf->dNum) {
					double *tmp = malloc(LINES_STEP * sizeof(double));
					assert(tmp != NULL);
					*(dlist[di++]) = tmp;
				}
				else {
					isError("create_LineFile, set large dlimit.");
				}
				break;
			case 3:
				if (ci < lf->cNum) {
					char *tmp = malloc(LINES_STEP * sizeof(char));
					assert(tmp != NULL);
					*(clist[ci++]) = tmp;
				}
				else {
					isError("create_LineFile, set large climit.");
				}
				break;
			case 4:
				if (li < lf->lNum) {
					long *tmp = malloc(LINES_STEP * sizeof(long));
					assert(tmp != NULL);
					*(llist[li++]) = tmp;
				}
				else {
					isError("create_LineFile, set large llimit.");
				}
				break;
			case 5:
				if (cci < lf->ccNum) {
					char **tmp = malloc(LINES_STEP * sizeof(void *));
					assert(tmp != NULL);
					*(cclist[cci++]) = tmp;
				}
				else {
					isError("create_LineFile, set large cclimit.");
				}
				break;
			default:
				isError("wrong type in init_memory_LineFile");
				break;
		}
	}
	lf->memNum += LINES_STEP;
}
//increase memory, not need typelist anymore, just check whether point is NULL or not.
static void add_memory_LineFile(struct LineFile *lf) {
	int ***ilist = lf->ilist;
	double ***dlist = lf->dlist;
	char ***clist = lf->clist;
	long ***llist = lf->llist;
	char ****cclist = lf->cclist;
	int i;
	for (i=0; i<lf->iNum; ++i) {
		if (*(ilist[i]) != NULL) {
			int *tmp = realloc(*(ilist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(int));
			assert(tmp != NULL);
			*(ilist[i]) = tmp;
		}
	}
	for (i=0; i<lf->dNum; ++i) {
		if (*(dlist[i]) != NULL) {
			double *tmp = realloc(*(dlist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(double));
			assert(tmp != NULL);
			*(dlist[i]) = tmp;
		}
	}
	for (i=0; i<lf->cNum; ++i) {
		if (*(clist[i]) != NULL) {
			char *tmp = realloc(*(clist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(char));
			assert(tmp != NULL);
			*(clist[i]) = tmp;
		}
	}
	for (i=0; i<lf->lNum; ++i) {
		if (*(llist[i]) != NULL) {
			long *tmp = realloc(*(llist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(long));
			assert(tmp != NULL);
			*(llist[i]) = tmp;
		}
	}
	for (i=0; i<lf->ccNum; ++i) {
		if (*(cclist[i]) != NULL) {
			char **tmp = realloc(*(cclist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(void *));
			assert(tmp != NULL);
			*(cclist[i]) = tmp;
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
static void set_lf_LineFile(struct LineFile *lf, char **allparts, int *typelist, int lread, int vn, char *isok) {
	int ***ilist = lf->ilist;
	double ***dlist = lf->dlist;
	char ***clist = lf->clist;
	long ***llist = lf->llist;
	char ****cclist = lf->cclist;
	int IL = 0;
	int DL = 0;
	int CL = 0;
	int LL = 0;
	int CCL = 0;
	int *ip;
	double *dp;
	char *cp;
	long *lp;
	char **ccp;

	int i,j;
	char *pend;
	for (i = 0; i < vn; ++i) {
		int type = typelist[i];
		char **p = allparts + i*LINES_READIN;
		switch(type) {
			case 1:
				ip = *(ilist[IL++]);
				for (j = 0; j < lread; ++j) {
					if (p[j] != NULL) {
						ip[j+lf->linesNum] = strtol(p[j], &pend, 10);
						if (pend[0]!='\0') {
							printf("create LineFile =>> %s file, line: %ld, i%d part.\n", lf->filename, j+lf->linesNum, IL);
							*isok = 0;
						}
					}
					else {
						ip[j+lf->linesNum] = -1;
					}
				}
				break;
			case 2:
				dp = *(dlist[DL++]);
				for (j = 0; j < lread; ++j) {
					if (p[j] != NULL) {
						dp[j+lf->linesNum] = strtod(p[j], &pend);
						if (pend[0]!='\0') {
							printf("create LineFile =>> %s file, line: %ld, d%d part.\n", lf->filename, j+lf->linesNum, IL);
							*isok = 0;
						}
					}
					else {
						dp[j+lf->linesNum] = -1;
					}
				}
				break;
			case 3:
				cp = *(clist[CL++]);
				for (j = 0; j < lread; ++j) {
					if (p[j] != NULL) {
						cp[j+lf->linesNum] = strtol(p[j], &pend, 10);
						if (pend[0]!='\0') {
							printf("create LineFile =>> %s file, line: %ld, d%d part.\n", lf->filename, j+lf->linesNum, IL);
							*isok = 0;
						}
					}
					else {
						cp[j+lf->linesNum] = -1;
					}
				}
				break;
			case 4:
				lp = *(llist[LL++]);
				for (j = 0; j < lread; ++j) {
					if (p[j] != NULL) {
						lp[j+lf->linesNum] = strtol(p[j], &pend, 10);
						if (pend[0]!='\0') {
							printf("create LineFile =>> %s file, line: %ld, d%d part.\n", lf->filename, j+lf->linesNum, IL);
							*isok = 0;
						}
					}
					else {
						lp[j+lf->linesNum] = -1;
					}
				}
				break;
			case 5:
				ccp = *(cclist[CCL++]);
				for (j = 0; j < lread; ++j) {
					if (p[j] != NULL) {
						int size = strlen(p[j]) + 1;
						ccp[j+lf->linesNum] = malloc(size*sizeof(char));
						assert(ccp[j+lf->linesNum] != NULL);
						memcpy(ccp[j+lf->linesNum], p[j], size);
					}
					else {
						ccp[j+lf->linesNum] = NULL;
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

	//the return lf.
	struct LineFile *lf = init_LineFile();

	//get typelist.
	int *typelist = malloc((lf->iNum + lf->dNum + lf->cNum + lf->lNum + lf->ccNum)*sizeof(int));
	assert(typelist != NULL);
	va_list vl;
	va_start(vl, filename);
	int vn = 0, type = -2;
	while (1 == (type = va_arg(vl, int)) || 2 == type || 3 == type || 4 == type || 5 == type) {
		typelist[vn++] = type;
	}
	va_end(vl);

	if (NULL == filename || 0 == vn || type != -1) {
		free(typelist);
		return lf;
	}
	lf->filename = filename;

	//check filename.
	FILE *fp = fopen(filename, "r");
	fileError(fp, "create_LineFile");

	//set lf memory with typelist.
	init_memory_LineFile(lf, vn, typelist);

	//buffer used to read file.
	char isok = 1;
	char *buffer = malloc(LINE_LENGTH * LINES_READIN * sizeof(char));
	assert(buffer != NULL);
	char **allparts = malloc(vn * LINES_READIN * sizeof(void *));
	assert(allparts != NULL);
	int lread = LINES_READIN;
	while (lread == LINES_READIN) {
		set_buffer_LineFile(fp, buffer, &lread);
		set_allparts_LineFile(buffer, allparts, vn, lread);
		while (lf->linesNum + lread > lf->memNum) {
			add_memory_LineFile(lf);
		}
		set_lf_LineFile(lf, allparts, typelist, lread, vn, &isok);
	}
	free(typelist);
	fclose(fp);
	free(buffer);
	free(allparts);

	if (!isok) {
		printf("create Linefile =>> %s has some non-valid lines, program stop.\n", lf->filename);
		exit(-1);
	}

	printf("create Linefile =>> read %s successfully.\n", lf->filename);
	return lf;
}

void free_LineFile(struct LineFile *lf) {
	int i;
	long j;
	for (i = 0; i < lf->iNum; ++i) {
		free(*(lf->ilist[i]));
	}
	for (i = 0; i < lf->dNum; ++i) {
		free(*(lf->dlist[i]));
	}
	for (i = 0; i < lf->cNum; ++i) {
		free(*(lf->clist[i]));
	}
	for (i = 0; i < lf->lNum; ++i) {
		free(*(lf->llist[i]));
	}
	for (i = 0; i < lf->ccNum; ++i) {
		if (*(lf->cclist[i]) != NULL) {
			for (j = 0; j < lf->linesNum; ++j) {
				free((*(lf->cclist[i]))[j]);
			}
		}
		free(*(lf->cclist[i]));
	}
	free(lf->ilist);
	free(lf->dlist);
	free(lf->clist);
	free(lf->llist);
	free(lf->cclist);
	free(lf);
}

void print_LineFile(struct LineFile *lf, char *filename) {
	if (NULL == lf) return;
	FILE *fp = fopen(filename, "w");
	fileError(fp, "print_LineFile");
	int ***ilist = lf->ilist;
	double ***dlist = lf->dlist;
	char ***clist = lf->clist;
	long ***llist = lf->llist;
	char ****cclist = lf->cclist;
	int i;
	long j;
	for (j = 0; j < lf->linesNum; ++j) {
		for (i=0; i<lf->iNum; ++i) {
			if (*(ilist[i]) != NULL) {
				fprintf(fp, "%d\t", (*(ilist[i]))[j]);
			}
		}
		for (i=0; i<lf->dNum; ++i) {
			if (*(dlist[i]) != NULL) {
				fprintf(fp, "%f\t", (*(dlist[i]))[j]);
			}
		}
		for (i=0; i<lf->cNum; ++i) {
			if (*(clist[i]) != NULL) {
				fprintf(fp, "%d\t", (*(clist[i]))[j]);
			}
		}
		for (i=0; i<lf->lNum; ++i) {
			if (*(llist[i]) != NULL) {
				fprintf(fp, "%ld\t", (*(llist[i]))[j]);
			}
		}
		for (i=0; i<lf->ccNum; ++i) {
			if (*(cclist[i]) != NULL) {
				fprintf(fp, "%s\t", (*(cclist[i]))[j]);
			}
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	printf("print LineFile =>> %s\n", filename);
}

struct LineFile *add_LineFile(struct LineFile *lf1, struct LineFile *lf2) {
	if (lf1 == NULL || lf2 == NULL) return NULL;

	struct LineFile *lf = init_LineFile();

	int ***ilist = lf->ilist;
	double ***dlist = lf->dlist;
	char ***clist = lf->clist;
	long ***llist = lf->llist;
	char ****cclist = lf->cclist;
	int ***ilist1 = lf1->ilist;
	double ***dlist1 = lf1->dlist;
	char ***clist1 = lf1->clist;
	long ***llist1 = lf1->llist;
	char ****cclist1 = lf1->cclist;
	int ***ilist2 = lf2->ilist;
	double ***dlist2 = lf2->dlist;
	char ***clist2 = lf2->clist;
	long ***llist2 = lf2->llist;
	char ****cclist2 = lf2->cclist;

	lf->linesNum = lf1->linesNum + lf2->linesNum;

	int i;
	long j;
	for (i=0; i<lf->iNum; ++i) {
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
	for (i=0; i<lf->dNum; ++i) {
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
	for (i=0; i<lf->cNum; ++i) {
		if (*(clist1[i]) != NULL && *(clist2[i]) != NULL) {
			*(clist[i]) = malloc((lf->linesNum)*sizeof(char));
			assert(*(clist[i]) != NULL);
			for (j = 0; j < lf1->linesNum; ++j) {
				(*(clist[i]))[j] = (*(clist1[i]))[j];
			}
			for (j = 0; j < lf2->linesNum; ++j) {
				(*(clist[i]))[j+lf1->linesNum] = (*(clist2[i]))[j];
			}
		}
		else if (*(clist1[i]) == NULL && *(clist2[i]) == NULL) {
			//*(ilist[i]) == NULL;
		}
		else {
			isError("lf1 and lf2 have different Structures, can not add lf1 with lf2.");
		}
	}
	for (i=0; i<lf->lNum; ++i) {
		if (*(llist1[i]) != NULL && *(llist2[i]) != NULL) {
			*(llist[i]) = malloc((lf->linesNum)*sizeof(long));
			assert(*(llist[i]) != NULL);
			for (j = 0; j < lf1->linesNum; ++j) {
				(*(llist[i]))[j] = (*(llist1[i]))[j];
			}
			for (j = 0; j < lf2->linesNum; ++j) {
				(*(llist[i]))[j+lf1->linesNum] = (*(llist2[i]))[j];
			}
		}
		else if (*(llist1[i]) == NULL && *(llist2[i]) == NULL) {
			//*(ilist[i]) == NULL;
		}
		else {
			isError("lf1 and lf2 have different Structures, can not add lf1 with lf2.");
		}
	}
	for (i=0; i<lf->ccNum; ++i) {
		if (*(cclist1[i]) != NULL && *(cclist2[i]) != NULL) {
			*(cclist[i]) = malloc((lf->linesNum)*sizeof(void *));
			assert(*(cclist[i]) != NULL);
			for (j = 0; j < lf1->linesNum; ++j) {
				int size = strlen((*(cclist1[i]))[j]) + 1;
				(*(cclist[i]))[j] = malloc(size*sizeof(char));
				assert((*(cclist[i]))[j] != NULL);
				memcpy((*(cclist[i]))[j], (*(cclist1[i]))[j], size*sizeof(char));
			}
			for (j = 0; j < lf2->linesNum; ++j) {
				int size = strlen((*(cclist2[i]))[j]) + 1;
				(*(cclist[i]))[j+lf1->linesNum] = malloc(size*sizeof(char));
				assert((*(cclist[i]))[j+lf1->linesNum] != NULL);
				memcpy((*(cclist[i]))[j+lf1->linesNum], (*(cclist2[i]))[j], size*sizeof(char));
			}
		}
		else if (*(cclist1[i]) == NULL && *(cclist2[i]) == NULL) {
			//*(ilist[i]) == NULL;
		}
		else {
			isError("lf1 and lf2 have different Structures, can not add lf1 with lf2.");
		}
	}

	lf->memNum = lf->linesNum;
	lf->filename = "addlinefile";
	return lf;
}
