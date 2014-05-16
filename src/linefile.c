#include "base.h"
#include "linefile.h"
#include "sort.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


#define LINES_STEP 1000000
#define LINE_LENGTH 2000
#define LINES_READIN 1000

//put i/d/c/l/ccx 's address into i/d/c/l/cclist
static void set_list_LineFile(struct LineFile *lf) {
	print4l("%s =>> begin......\n", __func__);
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
	print4l("%s =>> ......end.\n", __func__);
}
//create an empty but completive LineFile.
static struct LineFile *init_LineFile(void) {
	print4l("%s =>> begin......\n", __func__);
	struct LineFile *lf = smalloc(sizeof(struct LineFile));
	lf->linesNum = 0;
	lf->memNum = 0;
	lf->filename = "NewEmptyLF";

	lf->iNum = 9;
	lf->dNum = 9;
	lf->cNum = 9;
	lf->lNum = 9;
	lf->ccNum = 9;
	lf->ilist = smalloc(lf->iNum*sizeof(int **));
	lf->dlist = smalloc(lf->dNum*sizeof(void **));
	lf->clist = smalloc(lf->cNum*sizeof(void **));
	lf->llist = smalloc(lf->lNum*sizeof(void **));
	lf->cclist = smalloc(lf->ccNum*sizeof(void ***));

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
	print4l("%s =>> ......end.\n", __func__);
	return lf;
}
//alloc memory according to typelist.
static void init_memory_LineFile(struct LineFile *lf, int vn, int *typelist) {
	print4l("%s =>> begin......\n", __func__);
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
					*(ilist[ii++]) = smalloc(LINES_STEP * sizeof(int));
				}
				else {
					isError("%s =>> ilimit too small.", __func__);
				}
				break;
			case 2:
				if (di < lf->dNum) {
					*(dlist[di++]) = smalloc(LINES_STEP * sizeof(double));
				}
				else {
					isError("%s =>> dlimit too small.", __func__);
				}
				break;
			case 3:
				if (ci < lf->cNum) {
					*(clist[ci++]) = smalloc(LINES_STEP * sizeof(char));
				}
				else {
					isError("%s =>> climit too small.", __func__);
				}
				break;
			case 4:
				if (li < lf->lNum) {
					*(llist[li++]) = smalloc(LINES_STEP * sizeof(long));
				}
				else {
					isError("%s =>> llimit too small.", __func__);
				}
				break;
			case 5:
				if (cci < lf->ccNum) {
					*(cclist[cci++]) = smalloc(LINES_STEP * sizeof(char *));
				}
				else {
					isError("%s =>> cclimit too small.", __func__);
				}
				break;
			default:
				isError("%s =>> wrong type.");
				break;
		}
	}
	lf->memNum += LINES_STEP;
	print4l("%s =>> ......end.\n", __func__);
}
//increase memory, not need typelist anymore, just check whether point is NULL or not.
static void add_memory_LineFile(struct LineFile *lf) {
	print4l("%s =>> begin......\n", __func__);
	int ***ilist = lf->ilist;
	double ***dlist = lf->dlist;
	char ***clist = lf->clist;
	long ***llist = lf->llist;
	char ****cclist = lf->cclist;
	int i;
	for (i=0; i<lf->iNum; ++i) {
		if (*(ilist[i]) != NULL) {
			*(ilist[i]) = srealloc(*(ilist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(int));
		}
	}
	for (i=0; i<lf->dNum; ++i) {
		if (*(dlist[i]) != NULL) {
			*(dlist[i]) = srealloc(*(dlist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(double));
		}
	}
	for (i=0; i<lf->cNum; ++i) {
		if (*(clist[i]) != NULL) {
			*(clist[i]) = srealloc(*(clist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(char));
		}
	}
	for (i=0; i<lf->lNum; ++i) {
		if (*(llist[i]) != NULL) {
			*(llist[i]) = srealloc(*(llist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(long));
		}
	}
	for (i=0; i<lf->ccNum; ++i) {
		if (*(cclist[i]) != NULL) {
			*(cclist[i]) = srealloc(*(cclist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(void *));
		}
	}
	lf->memNum += LINES_STEP;
	print4l("%s =>> ......end.\n", __func__);
}
static void set_buffer_LineFile(FILE *fp, char *buffer, int *lread) {
	char *line = buffer;
	*lread = 0;
	while((*lread) != LINES_READIN && fgets(line, LINE_LENGTH, fp)) {
		line += LINE_LENGTH;
		++(*lread);
	}
	printnl("%s =>> read %d lines into buffer.\n", __func__, *lread);
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
	printnl("%s =>> distribute buffers into char *array, size: %d.\n", __func__, lread*vn);
}
static void set_lf_LineFile(struct LineFile *lf, char **allparts, int *typelist, int lread, int vn, char *isok) {
	int ***ilist = lf->ilist;
	double ***dlist = lf->dlist;
	char ***clist = lf->clist;
	long ***llist = lf->llist;
	char ****cclist = lf->cclist;
	int IL = 0, DL = 0, CL = 0, LL = 0, CCL = 0;
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
							print1l("%s =>> wrong line: \"%s\" file, line: %ld, i%d part.\n", __func__, lf->filename, j+lf->linesNum, IL);
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
							print1l("%s =>> wrong line: \"%s\" file, line: %ld, d%d part.\n", __func__, lf->filename, j+lf->linesNum, DL);
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
							print1l("%s =>> wrong line: \"%s\" file, line: %ld, c%d part.\n", __func__, lf->filename, j+lf->linesNum, CL);
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
							print1l("%s =>> wrong line: \"%s\" file, line: %ld, l%d part.\n", __func__, lf->filename, j+lf->linesNum, LL);
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
						ccp[j+lf->linesNum] = smalloc(size*sizeof(char));
						memcpy(ccp[j+lf->linesNum], p[j], size);
					}
					else {
						ccp[j+lf->linesNum] = NULL;
					}
				}
				break;
			default:
				isError("%s =>> wrong type.", __func__);
		}
	}
	lf->linesNum += lread;
	printnl("%s =>> already read in %ld lines.\n", __func__, lf->linesNum);
}

static char *typetype[] = {"int", "double", "char", "long", "c-string"};

struct LineFile *create_LineFile(char *filename, ...) {
	print2l("%s =>> begin......\n", __func__);

	//the return lf.
	struct LineFile *lf = init_LineFile();

	if (NULL == filename) {
		print3l("%s =>> NULL filename, return an empty linefile.\n", __func__);
		print2l("%s =>> ......end.\n", __func__);
		return lf;
	}

	//get typelist.
	int argMax = lf->iNum + lf->dNum + lf->cNum + lf->lNum + lf->ccNum;
	int *typelist = smalloc(argMax*sizeof(int));
	va_list vl;
	va_start(vl, filename);
	int vn = 0, type = -2;
	print3l("%s =>> detected line style: ", __func__);
	(void)typetype; //get rid of unused warning when VERBOSE_LEVEL is smaller than 3.
	while (1 == (type = va_arg(vl, int)) || 2 == type || 3 == type || 4 == type || 5 == type) {
		if (vn < argMax) {
			typelist[vn++] = type;
			print3l("%s ", typetype[type - 1]);
		}
		else {
			isError("%s =>> too much args.", __func__);
		}
	}
	print3l("\n");
	va_end(vl);

	if (0 == vn || type != -1) {
		free(typelist);
		print3l("%s =>> not valid types, return an empty linefile.\n", __func__);
		print2l("%s =>> ......end.\n", __func__);
		return lf;
	}
	lf->filename = filename;

	//check filename.
	FILE *fp = fopen(filename, "r");
	fileError(fp, "%s =>> can not open \"%s\" file.\n", __func__, filename);

	print3l("%s =>> open \"%s\" done.\n", __func__, filename);

	//set lf memory with typelist.
	init_memory_LineFile(lf, vn, typelist);
	print3l("%s =>> allocate the first piece of memory, now lf->memNum: %ld; lf->linesNum: %ld.\n", __func__, lf->memNum, lf->linesNum);

	//buffer used to read file.
	char isok = 1;
	char *buffer = smalloc(LINE_LENGTH * LINES_READIN * sizeof(char));
	char **allparts = smalloc(vn * LINES_READIN * sizeof(char *));
	int lread = LINES_READIN;
	while (lread == LINES_READIN) {
		set_buffer_LineFile(fp, buffer, &lread);
		set_allparts_LineFile(buffer, allparts, vn, lread);
		while (lf->linesNum + lread > lf->memNum) {
			add_memory_LineFile(lf);
			print3l("%s =>> allocate another piece of memory, now lf->memNum: %ld; lf->linesNum: %ld.\n", __func__, lf->memNum, lf->linesNum);
		}
		set_lf_LineFile(lf, allparts, typelist, lread, vn, &isok);
	}
	print3l("%s =>> totally read in %ld lines.\n", __func__, lf->linesNum);
	free(typelist);
	fclose(fp);
	free(buffer);
	free(allparts);

	isok == 0 ? isError("%s =>> file \"%s\" has some non-valid lines.", __func__, filename):1;

	print2l("%s =>> ......end.\n", __func__);
	return lf;
}

void free_LineFile(struct LineFile *lf) {
	print2l("%s =>> begin......\n", __func__);
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
	print2l("%s =>> ......end.\n", __func__);
}

void print_LineFile(struct LineFile *lf, char *filename) {
	print2l("%s =>> begin......\n", __func__);
	if (NULL == lf) {
		print3l("%s =>> lf == NULL, print nothing.\n", __func__);
		print2l("%s =>> ......end.\n", __func__);
		return;
	}
	FILE *fp = fopen(filename, "w");
	fileError(fp, "%s => can not \"%s\" file.\n", __func__, filename);
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
	print3l("%s =>> print LineFile into \"%s\".\n", __func__, filename);
	print2l("%s =>> ......end.\n", __func__);
}

struct LineFile *add_LineFile(struct LineFile *lf1, struct LineFile *lf2) {
	print2l("%s =>> begin......\n", __func__);
	if (lf1 == NULL || lf2 == NULL) {
		print3l("%s =>> lf1 or lf2 is NULL, return NULL.\n", __func__);
		print2l("%s =>> ......end.\n", __func__);
		return NULL;
	}

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
			*(ilist[i]) = smalloc((lf->linesNum)*sizeof(int));
			for (j = 0; j < lf1->linesNum; ++j) {
				(*(ilist[i]))[j] = (*(ilist1[i]))[j];
			}
			for (j = 0; j < lf2->linesNum; ++j) {
				(*(ilist[i]))[j+lf1->linesNum] = (*(ilist2[i]))[j];
			}
		}
		else if (*(ilist1[i]) == NULL && *(ilist2[i]) == NULL) {
		}
		else {
			isError("%s =>> lf1 and lf2 have different int Structures, can not add lf1 with lf2.\n", __func__);
		}
	}
	for (i=0; i<lf->dNum; ++i) {
		if (*(dlist1[i]) != NULL && *(dlist2[i]) != NULL) {
			*(dlist[i]) = smalloc((lf1->linesNum + lf2->linesNum)*sizeof(double));
			for (j = 0; j < lf1->linesNum; ++j) {
				(*(dlist[i]))[j] = (*(dlist1[i]))[j];
			}
			for (j = 0; j < lf2->linesNum; ++j) {
				(*(dlist[i]))[j+lf1->linesNum] = (*(dlist2[i]))[j];
			}
		}
		else if (*(dlist1[i]) == NULL && *(dlist2[i]) == NULL) {
		}
		else {
			isError("%s =>> lf1 and lf2 have different double Structures, can not add lf1 with lf2.\n", __func__);
		}
	}
	for (i=0; i<lf->cNum; ++i) {
		if (*(clist1[i]) != NULL && *(clist2[i]) != NULL) {
			*(clist[i]) = smalloc((lf->linesNum)*sizeof(char));
			for (j = 0; j < lf1->linesNum; ++j) {
				(*(clist[i]))[j] = (*(clist1[i]))[j];
			}
			for (j = 0; j < lf2->linesNum; ++j) {
				(*(clist[i]))[j+lf1->linesNum] = (*(clist2[i]))[j];
			}
		}
		else if (*(clist1[i]) == NULL && *(clist2[i]) == NULL) {
		}
		else {
			isError("%s =>> lf1 and lf2 have different char Structures, can not add lf1 with lf2.\n", __func__);
		}
	}
	for (i=0; i<lf->lNum; ++i) {
		if (*(llist1[i]) != NULL && *(llist2[i]) != NULL) {
			*(llist[i]) = smalloc((lf->linesNum)*sizeof(long));
			for (j = 0; j < lf1->linesNum; ++j) {
				(*(llist[i]))[j] = (*(llist1[i]))[j];
			}
			for (j = 0; j < lf2->linesNum; ++j) {
				(*(llist[i]))[j+lf1->linesNum] = (*(llist2[i]))[j];
			}
		}
		else if (*(llist1[i]) == NULL && *(llist2[i]) == NULL) {
		}
		else {
			isError("%s =>> lf1 and lf2 have different long Structures, can not add lf1 with lf2.\n", __func__);
		}
	}
	for (i=0; i<lf->ccNum; ++i) {
		if (*(cclist1[i]) != NULL && *(cclist2[i]) != NULL) {
			*(cclist[i]) = smalloc((lf->linesNum)*sizeof(void *));
			for (j = 0; j < lf1->linesNum; ++j) {
				int size = strlen((*(cclist1[i]))[j]) + 1;
				(*(cclist[i]))[j] = smalloc(size*sizeof(char));
				memcpy((*(cclist[i]))[j], (*(cclist1[i]))[j], size*sizeof(char));
			}
			for (j = 0; j < lf2->linesNum; ++j) {
				int size = strlen((*(cclist2[i]))[j]) + 1;
				(*(cclist[i]))[j+lf1->linesNum] = smalloc(size*sizeof(char));
				memcpy((*(cclist[i]))[j+lf1->linesNum], (*(cclist2[i]))[j], size*sizeof(char));
			}
		}
		else if (*(cclist1[i]) == NULL && *(cclist2[i]) == NULL) {
		}
		else {
			isError("%s =>> lf1 and lf2 have different c-string Structures, can not add lf1 with lf2.\n", __func__);
		}
	}

	lf->memNum = lf->linesNum;
	lf->filename = "add_linefile";
	print2l("%s =>> ......end.\n", __func__);
	return lf;
}

struct LineFile *clone_LineFile(struct LineFile *lf) {
	print2l("%s =>> begin......\n", __func__);
	if (lf == NULL) {
		print3l("%s =>> source LineFile is NULL, return NULL.\n", __func__);
		print2l("%s =>> ......end.\n", __func__);
		return NULL;
	}

	int ***ilist 	= 	lf->ilist;
	double ***dlist = 	lf->dlist;
	char ***clist 	= 	lf->clist;
	long ***llist 	= 	lf->llist;
	char ****cclist = 	lf->cclist;

	struct LineFile *newlf = init_LineFile();
	int ***ilist_n = 	newlf->ilist;
	double ***dlist_n = 	newlf->dlist;
	char ***clist_n = 	newlf->clist;
	long ***llist_n = 	newlf->llist;
	char ****cclist_n = 	newlf->cclist;

	newlf->linesNum = lf->linesNum;

	int i;
	long j;
	for (i=0; i<lf->iNum; ++i) {
		if (*(ilist[i]) != NULL) {
			*(ilist_n[i]) = smalloc((lf->linesNum)*sizeof(int));
			for (j = 0; j < lf->linesNum; ++j) {
				(*(ilist_n[i]))[j] = (*(ilist[i]))[j];
			}
		}
	}
	for (i=0; i<lf->dNum; ++i) {
		if (*(dlist[i]) != NULL) {
			*(dlist_n[i]) = smalloc((lf->linesNum)*sizeof(double));
			for (j = 0; j < lf->linesNum; ++j) {
				(*(dlist_n[i]))[j] = (*(dlist[i]))[j];
			}
		}
	}
	for (i=0; i<lf->cNum; ++i) {
		if (*(clist[i]) != NULL) {
			*(clist_n[i]) = smalloc((lf->linesNum)*sizeof(char));
			for (j = 0; j < lf->linesNum; ++j) {
				(*(clist_n[i]))[j] = (*(clist[i]))[j];
			}
		}
	}
	for (i=0; i<lf->lNum; ++i) {
		if (*(llist[i]) != NULL) {
			*(llist_n[i]) = smalloc((lf->linesNum)*sizeof(long));
			for (j = 0; j < lf->linesNum; ++j) {
				(*(llist_n[i]))[j] = (*(llist[i]))[j];
			}
		}
	}
	for (i=0; i<lf->ccNum; ++i) {
		if (*(cclist[i]) != NULL) {
			*(cclist_n[i]) = smalloc((lf->linesNum)*sizeof(void *));
			for (j = 0; j < lf->linesNum; ++j) {
				int size = strlen((*(cclist[i]))[j]) + 1;
				(*(cclist_n[i]))[j] = smalloc(size*sizeof(char));
				memcpy((*(cclist_n[i]))[j], (*(cclist[i]))[j], size*sizeof(char));
			}
		}
	}

	newlf->memNum = newlf->linesNum;
	newlf->filename = "clone_linefile";
	print2l("%s =>> ......end.\n", __func__);
	return lf;
}
