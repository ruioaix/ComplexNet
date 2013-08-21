#include "../inc/complexnet_hashtable.h"

//recursion free.
void freeHashElement(struct HashElement *he)
{
	if (he != NULL && he->next !=NULL) {
		freeHashElement(he->next);
	}
	free(he);
	he=NULL;
}

void freeHashTable(struct HashTable *ht)
{
	elementNumBackHT(ht);
	int i;
	for (i=0; i<ht->length; ++i) {
		if (ht->elementNum[i]>0) {
			freeHashElement(ht->he[i]);
		}
	}
	free(ht->elementNum);
	free(ht->he);
	free(ht);
}


struct HashTable *createHashTable(int length)
{
	struct HashTable *ht=calloc(1, sizeof(struct HashTable));
	assert(ht!=NULL);

	struct HashElement **he=calloc(length, sizeof(void *));
	assert(he!=NULL);

	int *elementNum=calloc(length, sizeof(int));
	assert(elementNum!=NULL);
	
	ht->he=he;
	ht->length=length;
	ht->elementNum=elementNum;
	//ht->sumSign=0;

	return ht;
}

void insertHEtoHT(struct HashTable *ht, long element)
{
	elementNumBackHT(ht);

	struct HashElement *he=calloc(1, sizeof(struct HashElement));
	assert(he!=NULL);

	int index=element%ht->length;
	he->element=element;

	struct HashElement *l;
	struct HashElement *r;

	if (ht->he[index]!=NULL) {
		//if element is smaller than the first element, insert.
		if (ht->he[index]->element > element) {
			he->next=ht->he[index];
			ht->he[index]=he;
			++ht->elementNum[index];
		} else if (ht->he[index]->element < element) {
			//if element is bigger than the first element, and there is only one element, insert.
			if (ht->he[index]->next == NULL) {
				he->next=NULL;
				ht->he[index]->next=he;
				++ht->elementNum[index];
			} else {
				l=ht->he[index];
				r=ht->he[index]->next;
				while(r) {
					//if element is bigger than the last element, insert.
					if (r->element < element) {
						if (r->next==NULL) {
							he->next=NULL;
							r->next=he;
							++ht->elementNum[index];
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
						++ht->elementNum[index];
						break;
					//if element existed, do nothing.
					} else {
						free(he);
						break;
					}
				}
			}
		} else {
			free(he);
		}
	} else {
		//if no element, insert.
		he->next=NULL;
		ht->he[index]=he;
		++ht->elementNum[index];
	}
}

void deleteHEfromHT(struct HashTable *ht, long element)
{
	elementNumBackHT(ht);
	int index=element%ht->length;
	struct HashElement *l;
	struct HashElement *r;
	if (ht->he[index]!=NULL) {
		if (ht->he[index]->element==element) {
			struct HashElement *temp=ht->he[index];
			ht->he[index]=ht->he[index]->next;
			free(temp);
			--ht->elementNum[index];
		} 
		else if (ht->he[index]->next!=NULL){
			l=ht->he[index];
			r=ht->he[index]->next;
			while(r) {
				if (r->element==element) {
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

void elementNumSumHT(struct HashTable *ht)
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
		ht->sumSign=1;
	}
}

void elementNumBackHT(struct HashTable *ht)
{
	if (ht->sumSign) {
		int i;
		for (i=1; i<ht->length; ++i) {
			ht->elementNum[i-1]=ht->elementNum[i]-ht->elementNum[i-1];
		}
		ht->elementNum[ht->length-1]=0;
		struct HashElement *he=ht->he[ht->length-1];
		while(he) {
			++ht->elementNum[ht->length-1];
			he=he->next;
		}
		ht->sumSign=0;
	}
}

int getelementIndexHT(struct HashTable *ht, long element)
{
	elementNumSumHT(ht);
	int rindex=element%ht->length;
	struct HashElement *r=ht->he[rindex];
	int index=ht->elementNum[rindex];
	while(r) {
		if(r->element == element) {
			return index;
		}
		++index;
		r=r->next;
	}
	return -1;
}
