#ifndef COMPLEXNET_BIPARTITE_H
#define COMPLEXNET_BIPARTITE_H

#include "../inc/complexnet_linefile.h"

//this struct is just one way for describing Bipartite.
//other ways like (int ***xx) is also usefully.
//Bipartite contains two parts. e.g. user and item.
//but here, create_Bipartite only create user Bipartite or item Bipartite.
//if you want both, create two time with different i1toi2 arg.
struct Bipartite {
	int maxId;
	int minId;
	int idNum;
	long countMax;
	long countMin;
	long *count;
	int **id;
	int **i3;
	int **i4;
	long edgesNum;
};

void free_Bipartite(struct Bipartite *bip);
//if i1toi2 is not zero, means the  column 1 is the index, and column 2 is the data saved into (int **id).
//column 1 and column 2 is the data in origin file.
//if i1toi2 is zero, means column 2 is the index.
struct Bipartite *create_Bipartite(const struct i4LineFile * const file, int i1toi2);

//abstract_Bipartite will abstract one edge from each user or item and return.
//the arg bip which has been abstracted will be changed, all abstracted edges will be deleted in the arg bip.
struct i4LineFile *abstract_Bipartite(struct Bipartite *bip);
struct i4LineFile *backtofile_Bipartite(struct Bipartite *bip);

void deleteid_Bipartite(struct Bipartite *bip, int id);
//write a file from a bip.
void print_Bipartite(struct Bipartite *bip, char *filename);

#endif
