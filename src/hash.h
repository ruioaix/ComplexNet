#ifndef CN_HASHTABLE_H
#define CN_HASHTABLE_H

#include "linefile.h"

struct HE {
	long element;
	struct HE *next;
};

struct HT {
	int height;
	int idNum;
	int *count;
	struct HE **he;
};

//this hashtable is use to convert vtId to 0--vtNum-1. 
//the resulted vtId will be continuous and swquential. like 0,1,2...3212398.
//you can insert all old vtId into the hashtable, then with using xxx, you can get new vtId(the index).
struct HT *create_HT(int length);

//free
void free_HT(struct HT *ht);

//insert and delete
int insert_HE_HT(struct HT *ht, long element);
void delete_HE_HT(struct HT *ht, long element);


int get_index_HT(struct HT *ht, long element);
//
//void wc_ii_HT(struct LineFile *file, char *filename);

#endif
