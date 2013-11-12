#include "../../inc/hashtable/dmp.h"
#include "../../inc/utility/error.h"
#include "../../inc/linefile/iilinefile.h"
#include "../../inc/linefile/i4linefile.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static int HASHARG = 19;
//elementNum
static void elementNumSumHT(struct HashTable_DMP *ht)
{
	if (!ht->sumSign) {
		int i;
		int ivalue=ht->elementNum[0];
		ht->elementNum[0]=0;
		for (i=1; i<ht->length; ++i) {
			int temp=ht->elementNum[i];
			ht->elementNum[i]=ivalue+ht->elementNum[i-1];
			ivalue=temp;
		}
		ht->elementSumNum = ht->elementNum[ht->length-1];
		struct HashElement_DMP *he=ht->he[ht->length-1];
		while(he) {
			++ht->elementSumNum;
			he=he->next;
		}
		ht->sumSign=1;
	}
}

static void elementNumBackHT(struct HashTable_DMP *ht)
{
	if (ht->sumSign) {
		int i;
		for (i=1; i<ht->length; ++i) {
			ht->elementNum[i-1]=ht->elementNum[i]-ht->elementNum[i-1];
		}
		ht->elementNum[ht->length-1]=0;
		struct HashElement_DMP *he=ht->he[ht->length-1];
		while(he) {
			++ht->elementNum[ht->length-1];
			he=he->next;
		}
		ht->sumSign=0;
	}
}

long getelementSumNumHT_DMP(struct HashTable_DMP *ht) {
	elementNumSumHT(ht);
	return ht->elementSumNum;
}

struct HashElement_DMP *getHEfromHT_DMP(struct HashTable_DMP *ht, int i1, int i2) {
	elementNumBackHT(ht);
	int index=(i2*HASHARG+i1)%ht->length;
	struct HashElement_DMP *r;
	if (ht->he[index]!=NULL) {
		if (ht->he[index]->i1 == i1 && ht->he[index]->i2 == i2) {
			return ht->he[index];
		} 
		else if (ht->he[index]->next!=NULL){
			r=ht->he[index]->next;
			while(r) {
				if (r->i1 == i1 && r->i2 == i2) {
					return r;
				}
				r=r->next;
			}
		}
	}
	return NULL;
}

//recursion free.
static void free_HashElement_DMP(struct HashElement_DMP *he)
{
	if (he != NULL && he->next !=NULL) {
		free_HashElement_DMP(he->next);
	}
	free(he);
	he=NULL;
}

void free_HashTable_DMP(struct HashTable_DMP *ht)
{
	elementNumBackHT(ht);
	int i;
	for (i=0; i<ht->length; ++i) {
		if (ht->elementNum[i]>0) {
			free_HashElement_DMP(ht->he[i]);
		}
	}
	free(ht->elementNum);
	free(ht->he);
	free(ht);
}

struct HashTable_DMP *create_HashTable_DMP(int length)
{
	struct HashTable_DMP *ht=malloc(sizeof(struct HashTable_DMP));
	assert(ht!=NULL);

	struct HashElement_DMP **he=malloc(length*sizeof(void *));
	assert(he!=NULL);

	int i;
	for (i=0; i<length; ++i) {
		he[i] = NULL;
	}

	int *elementNum=calloc(length, sizeof(int));
	assert(elementNum!=NULL);
	
	ht->he=he;
	ht->length=length;
	ht->elementNum=elementNum;
	ht->infect_source = -1;
	ht->sumSign = 0;
	ht->elementSumNum = -1;

	return ht;
}

int insertHEtoHT_DMP(struct HashTable_DMP *ht, int i1, int i2, double theta, double phi, double p1, double p2)
{
	elementNumBackHT(ht);

	struct HashElement_DMP *he=malloc(sizeof(struct HashElement_DMP));
	assert(he!=NULL);

	int index=(i2*HASHARG + i1)%ht->length;
	he->i1=i1;
	he->i2 = i2;
	he->theta = theta;
	he->phi = phi;
	he->p1 = p1;
	he->p2 = p2;

	he->next=ht->he[index];
	ht->he[index]=he;
	++ht->elementNum[index];

	return 0;
}

void deleteHEfromHT_DMP(struct HashTable_DMP *ht, int i1, int i2)
{
	elementNumBackHT(ht);
	int index=(i2*HASHARG+i1)%ht->length;
	struct HashElement_DMP *l;
	struct HashElement_DMP *r;
	if (ht->he[index]!=NULL) {
		if (ht->he[index]->i1 == i1 && ht->he[index]->i2 == i2) {
			struct HashElement_DMP *temp=ht->he[index];
			ht->he[index]=ht->he[index]->next;
			free(temp);
			--ht->elementNum[index];
		} 
		else if (ht->he[index]->next!=NULL){
			l=ht->he[index];
			r=ht->he[index]->next;
			while(r) {
				if (r->i1 == i1 && r->i2 == i2) {
					l->next=r->next;
					free(r);
					--ht->elementNum[index];
					break;
				}
				l=r;
				r=r->next;
			}
		}
	}
}


void clean_HashTable_DMP(struct HashTable_DMP *ht, int infect_source) {
	int i;
	struct HashElement_DMP *l;
	for (i=0; i<ht->length; ++i) {
		l = ht->
	}
}

void init_HashTable_DMP(struct HashTable_DMP *ht, struct iiLineFile *file, int infect_source) {
	//init the P1 P2 Theta Phi
	long i;
	int i1, i2;
	struct iiLine *lines = file->lines;
	for (i=0; i<file->linesNum; ++i) {
		i1 = lines[i].i1;
		i2 = lines[i].i2;
		//i is S
		if (i1 != infect_source) {
			insertHEtoHT_DMP(ht, i1, i2, 1, 0, 1, 0);
		}
		//i is I
		else {
			insertHEtoHT_DMP(ht, i1, i2, 1, 1, 0, 0);
		}

		if (i2 != infect_source) {
			insertHEtoHT_DMP(ht, i2, i1, 1, 0, 1, 0);
		}
		else {
			insertHEtoHT_DMP(ht, i2, i1, 1, 1, 0, 0);
		}
	}
}

void print_HashTable_DMP(struct HashTable_DMP *ht, char *filename) {
	FILE *fp;
	fp = fopen(filename, "w");
	fileError(fp, "print_HashTable_DMP");
	elementNumBackHT(ht);
	int i;
	struct HashElement_DMP *l;
	for (i=0; i< ht->length; ++i) {
		l = ht->he[i];
		while(l != NULL) {
			fprintf(fp, "%d\t%d\n", l->i1, l->i2);;	
			l = l->next;
		}
	}
	fclose(fp);
	printf("print_HashTable_DMP done.\n");
}

void print_ENum_HashTable_DMP(struct HashTable_DMP *ht, char *filename) {
	FILE *fp;
	fp = fopen(filename, "w");
	fileError(fp, "print_ENum_HashTable_DMP");
	int i;
	elementNumBackHT(ht);
	int elementNumMax = -1;
	for (i=0; i< ht->length; ++i) {
		fprintf(fp, "%d, %d\n", i, ht->elementNum[i]);	
		elementNumMax = elementNumMax > ht->elementNum[i] ? elementNumMax : ht->elementNum[i];
	}
	printf("print_ENum_HashTable_DMP done\n\telementNumMax: %d\n", elementNumMax);
	fclose(fp);
}

double *dmp(struct HashTable_DMP *ht, int infect_source, double infect_rate, double recover_rate, int T, struct Net *net) {
	if (infect_source > net->maxId || net->count[infect_source] == 0) {
		printf("dmp error\n\twrong infect_source: %d\n", infect_source);
		return NULL;
	}
	int maxId = net->maxId;
	int i;
	long j, k;
	int step = 1;
	struct HashElement_DMP *l;
	
	double *PS = malloc(3*(net->maxId+1)*sizeof(double));
	assert(PS != NULL);
	double *PI = PS + net->maxId + 1;
	double *PR = PI + net->maxId + 1;

	//init PS PI PR
	for (i=0; i<net->maxId+1; ++i) {
		PR[i] = 0;
		if (i!=infect_source) {
			PS[i] = 1;
			PI[i] = 0;
		}
		else {
			PS[i] = 0;
			PI[i] = 1;
		}
	}

	double *theta_temp = calloc(net->maxId + 1, sizeof(double));

	while (step <= T) {
		printf("step: %d\n", step);fflush(stdout);
		//compute theta, phi, PSi->j
		for (i=0; i<ht->length; ++i) {
			l = ht->he[i];
			while(l) {
				l->theta = l->theta - infect_rate*l->phi;
				l = l->next;
			}
		}
		printf("\ttheta done.\n");fflush(stdout);
		for (i=0; i<maxId+1; ++i) {
			for (j=0; j<net->count[i]; ++j) {
				l = getHEfromHT_DMP(ht, net->edges[i][j], i);
				theta_temp[net->edges[i][j]] = l->theta;
			}

			if (i != infect_source) {
				for (j=0; j<net->count[i]; ++j) {
					l = getHEfromHT_DMP(ht, i, net->edges[i][j]);	
					l->p2 = 1;
					for (k=0; k<net->count[i]; ++k) {
						if (j != k) {
							l->p2 *= theta_temp[net->edges[i][k]];
						}
					}
				}
			}
			else {
				for (j=0; j<net->count[i]; ++j) {
					l = getHEfromHT_DMP(ht, i, net->edges[i][j]);
					l->p2 = 0;
				}
			}
			if (i%1000 == 0) {printf("%d\n", i);fflush(stdout);}
		}
		printf("\tp2 done.\n");fflush(stdout);
		for (i=0; i<maxId+1; ++i) {
			for (j=0; j<net->count[i]; ++j) {
				l = getHEfromHT_DMP(ht, i, net->edges[i][j]);
				l->phi = (1-infect_rate)*(1-recover_rate)*l->phi + l->p1 - l->p2;
				double temp;
				temp = l->p2;
				l->p2 = l->p1;
				l->p1 = temp;
			}
		}
		printf("\tphi done.\n");fflush(stdout);

		//compute and store PS PI PR.
		for (i=0; i<maxId+1; ++i) {
			if (i!=infect_source) {
				PS[i] = 1;
				for (j=0; j<net->count[i]; ++j) {
					int jj = net->edges[i][j];
					l = getHEfromHT_DMP(ht, jj, i);
					if (!l) isError("PS index == -1");
					PS[i] *= l->theta;
				}
			}
			else {
				PS[i] = 0;
			}
			PR[i] = PR[i] + recover_rate*PI[i];
			PI[i] = 1 - PS[i] - PR[i];
		}
		printf("\tpspipr done.\n");fflush(stdout);
		
		++step;
	}
	return PS;
}
