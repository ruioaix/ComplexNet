#ifndef DMP_H
#define DMP_H

#include "../linefile/iilinefile.h"
#include "../compact/net.h"

struct HashElement_DMP {
	int i1;
	int i2;
	double theta;
	double phi;
	double p1;
	double p2;
	struct HashElement_DMP *next;
};

struct HashTable_DMP {
	int length;
	struct HashElement_DMP **he;

	char sumSign;
	long elementSumNum;
	int *elementNum;

	int infect_source;
};

//this hashtable is use to convert vtId to 0--vtNum-1. 
//the resulted vtId will be continuous and swquential. like 0,1,2...3212398.
//you can insert every old vtId into the hashtable, then elementNumSumHT, then you can get every old vtId's new vtId(the index).
struct HashTable_DMP *create_HashTable_DMP(int length);

//free
void free_HashTable_DMP(struct HashTable_DMP *ht);

//insert and delete
int insertHEtoHT_DMP(struct HashTable_DMP *ht, int i1, int i2, double theta, double phi, double p1, double p2);
void deleteHEfromHT_DMP(struct HashTable_DMP *ht, int i1, int i2);

//get
struct HashElement_DMP *getHEfromHT_DMP(struct HashTable_DMP *ht, int i1, int i2);
long getelementSumNumHT_DMP(struct HashTable_DMP *ht);

//print
void print_ENum_HashTable_DMP(struct HashTable_DMP *ht, char *filename);
void print_HashTable_DMP(struct HashTable_DMP *ht, char *filename);

//
void init_HashTable_DMP(struct HashTable_DMP *ht, struct iiLineFile *file, int infect_source);
double *dmp(struct HashTable_DMP *ht, int infect_source, double infect_rate, double recover_rate, int T, struct Net *net);

#endif
