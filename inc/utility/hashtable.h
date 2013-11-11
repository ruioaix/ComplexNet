#ifndef HASHTABLE_H
#define HASHTABLE_H

struct HashElement {
	long element;
	struct HashElement *next;
};

struct HashTable {
	int length;
	char sumSign;
	int *elementNum;
	struct HashElement **he;
	int elementSumNum;

	long *index;
	char indexSign;
};

//this hashtable is use to convert vtId to 0--vtNum-1. 
//the resulted vtId will be continuous and swquential. like 0,1,2...3212398.
//you can insert every old vtId into the hashtable, then elementNumSumHT, then you can get every old vtId's new vtId(the index).
struct HashTable *createHashTable(int length);
//free
//void freeHashElement(struct HashElement *he);
void freeHashTable(struct HashTable *ht);

//insert and delete
int insertHEtoHT(struct HashTable *ht, long element);
void setelementIndexHT(struct HashTable *ht);
void deleteHEfromHT(struct HashTable *ht, long element);
//get index
int getelementIndexHT(struct HashTable *ht, long element);
int getelementSumNumHT(struct HashTable *ht);
long getelementValueHT(struct HashTable *ht, int index);

//thread routine, arg type is iiLineFile.
void *writeContinuousiiLineFileHT(void * arg);
void *writeContinuousi4LineFileHT(void * arg);

#endif
