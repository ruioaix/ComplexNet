#include "base.h"
#include "hash.h"
#include "linefile.h"
#include <assert.h>
#include <stdlib.h>

struct HT *create_HT(int height)
{
	struct HT *ht=malloc(sizeof(struct HT));
	assert(ht!=NULL);
	ht->height = height;
	ht->idNum = 0;

	ht->he=malloc(height*sizeof(void *));
	int i;
	for (i = 0; i < height; ++i) {
		ht->he[i] = NULL;
	}

	ht->count=calloc(height, sizeof(int));
	assert(ht->count!=NULL);

	return ht;
}

//recursion free.
static void free_HE(struct HE *he)
{
	if (he != NULL && he->next !=NULL) {
		free_HE(he->next);
	}
	free(he);
	he=NULL;
}

void free_HT(struct HT *ht)
{
	int i;
	for (i=0; i<ht->height; ++i) {
		free_HE(ht->he[i]);
	}
	free(ht->count);
	free(ht->he);
	free(ht);
}

int insert_HE_HT(struct HT *ht, long element)
{
	struct HE *he=malloc(sizeof(struct HE));
	assert(he!=NULL);
	he->element=element;

	int index=element%ht->height;

	struct HE *l;
	struct HE *r;

	if (ht->he[index]!=NULL) {
		//if element is smaller than the first element, insert.
		if (ht->he[index]->element > element) {
			he->next=ht->he[index];
			ht->he[index]=he;
			++ht->count[index];
		} else if (ht->he[index]->element < element) {
			//if element is bigger than the first element, and there is only one element, insert.
			if (ht->he[index]->next == NULL) {
				he->next=NULL;
				ht->he[index]->next=he;
				++ht->count[index];
			} else {
				l=ht->he[index];
				r=ht->he[index]->next;
				while(r != NULL) {
					//if element is bigger than the last element, insert.
					if (r->element < element) {
						if (r->next==NULL) {
							he->next=NULL;
							r->next=he;
							++ht->count[index];
							break;
						//if element is bigger than any element but not the last one, next.
						} else {
							l=r;
							r=r->next;	
							continue;
						}
					//else if element is smaller than any element, insert.
					} else if (r->element > element) {
						he->next=l->next;
						l->next=he;
						++ht->count[index];
						break;
					//if element existed, do nothing.
					} else {
						free(he);
						return -1;
					}
				}
			}
		} else {
			free(he);
			return -1;
		}
	} else {
		//if no element, insert.
		he->next=NULL;
		ht->he[index]=he;
		++ht->count[index];
	}
	return 0;
}

void delete_HE_HT(struct HT *ht, long element)
{
	int index=element%ht->height;
	struct HE *l;
	struct HE *r;
	if (ht->he[index]!=NULL) {
		if (ht->he[index]->element==element) {
			struct HE *temp=ht->he[index];
			ht->he[index]=ht->he[index]->next;
			free(temp);
			--ht->count[index];
		} 
		else if (ht->he[index]->next!=NULL){
			l=ht->he[index];
			r=ht->he[index]->next;
			while(r != NULL) {
				if (r->element==element) {
					l->next=r->next;
					free(r);
					--ht->count[index];
					break;
				}
				l=r;
				r=r->next;
			}
		}
	}
}

int get_index_HT(struct HT *ht, long element)
{
	int rindex=element%ht->height;
	int i;
	int index = 0;
	for (i = 0; i < rindex; ++i) {
		index += ht->count[i];	
	}
	struct HE *r=ht->he[rindex];
	while(r) {
		if(r->element == element) {
			return index;
		}
		++index;
		r=r->next;
	}
	return -1;
}

/*
//count
static void countSumHT(struct HT *ht)
{
	if (!ht->sumSign) {
		int i;
		int ivalue=ht->count[0];
		ht->count[0]=0;
		for (i=1; i<ht->height; ++i) {
			int temp=ht->count[i];
			ht->count[i]=ivalue+ht->count[i-1];
			ivalue=temp;
		}
		ht->elementSumNum = ht->count[ht->height-1];
		struct HE *he=ht->he[ht->height-1];
		while(he) {
			++ht->elementSumNum;
			he=he->next;
		}
		ht->sumSign=1;
	}
}

static void countBackHT(struct HT *ht)
{
	if (ht->sumSign) {
		int i;
		for (i=1; i<ht->height; ++i) {
			ht->count[i-1]=ht->count[i]-ht->count[i-1];
		}
		ht->count[ht->height-1]=0;
		struct HE *he=ht->he[ht->height-1];
		while(he) {
			++ht->count[ht->height-1];
			he=he->next;
		}
		ht->sumSign=0;
	}
}


int getelementSumNumHT(struct HT *ht) {
	countSumHT(ht);
	return ht->elementSumNum;
}

void setelementIndexHT(struct HT *ht) {
	if (!ht->indexSign) {
		countSumHT(ht);
		long *index = malloc((ht->elementSumNum)*sizeof(long));
		assert(index != NULL);

		struct HE *he;
		int i, j=0;
		for (i=0; i<ht->height; ++i) {
			he = ht->he[i];
			while(he) {
				index[j++] = he->element;
				he=he->next;
				//if (j>=ht->elementSumNum) {
				//	printf("%d\n", j);fflush(stdout);
				//}
			}
		}
		ht->index = index;
		ht->indexSign = 1;
	}
}

long getelementValueHT(struct HT *ht, int index) {
	if (ht->indexSign) {
		return ht->index[index];
	}
	else {
		countSumHT(ht);
		if (index>ht->elementSumNum) return -1;
		int i;
		for (i=0; i<ht->height; ++i) {
			if (ht->count[i]<=index) {
				continue;
			}
			break;
		}
		int rowindex = index - ht->count[--i];
		struct HE *he = ht->he[i];
		while(rowindex--) {
			he=he->next;
		}
		return he->element;
	}
}





void *writeContinuousiiLineFileHT(void * arg) {
	struct iiLineFile *file = arg;
	struct HT *ht = createHT(3000000);
	long i;
	for (i=0; i<file->linesNum; ++i) {
		insertHEtoHT(ht, file->lines[i].i1);
		insertHEtoHT(ht, file->lines[i].i2);
	}
	FILE *fp = fopen("data/continuousiiLineFile", "w");
	fileError(fp, "data/continuousiiLineFile");
	for (i=0; i<file->linesNum; ++i) {
		int i1 = getelementIndexHT(ht, file->lines[i].i1);
		int i2 = getelementIndexHT(ht, file->lines[i].i2);
		fprintf(fp, "%d\t%d\n", i1, i2);
	}
	fclose(fp);
	freeHT(ht);
	return (void *)0;
}

void *writeContinuousi4LineFileHT(void * arg) {
	struct i4LineFile *file = arg;
	struct HT *ht = createHT(3000000);
	//struct HT *ht_i4 = createHT(300000);
	long i;

	for (i=0; i<file->linesNum; ++i) {
		insertHEtoHT(ht, file->lines[i].i1);
		insertHEtoHT(ht, file->lines[i].i2);
	//	insertHEtoHT(ht_i4,file->lines[i].i4);
	}
	FILE *fp = fopen("data/continuousi4LineFile", "w");
	fileError(fp, "data/continuousi4LineFile");
	for (i=0; i<file->linesNum; ++i) {
		int i1 = getelementIndexHT(ht, file->lines[i].i1);
		int i2 = getelementIndexHT(ht, file->lines[i].i2);
	//	int i4 = getelementIndexHT(ht_i4, file->lines[i].i4);
	//	fprintf(fp, "%d\t%d\t%d\t%d\n", i1, i2, file->lines[i].i3, i4);
		fprintf(fp, "%d\t%d\t%d\t%d\n", i1, i2, file->lines[i].i3, file->lines[i].i4);
	}
	fclose(fp);
	freeHT(ht);
	//freeHT(ht_i4);
	return (void *)0;
}


void wc_ii_HT(struct iiLineFile *file, char *filename) {
	struct HT *i1 = createHT(3000000);
	struct HT *i2 = createHT(3000000);
	long i;
	for (i=0; i<file->linesNum; ++i) {
		insertHEtoHT(i1, file->lines[i].i1);
		insertHEtoHT(i2, file->lines[i].i2);
	}
	FILE *fp = fopen(filename, "w");
	fileError(fp, "wc_bip2_ii_HT");
	for (i=0; i<file->linesNum; ++i) {
		int ii1 = getelementIndexHT(i1, file->lines[i].i1);
		int ii2 = getelementIndexHT(i2, file->lines[i].i2);
		fprintf(fp, "%d\t%d\n", ii1, ii2);
	}
	fclose(fp);
	freeHT(i1);
	freeHT(i2);
}
*/
