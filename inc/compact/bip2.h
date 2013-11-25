/**
 * bip2 means functions in this file only take care of the basic bipartite, 2 part.
 * e.g. user and item. no any other information, like the score which an user give to an item.
 *
 * the origin bipartite is coming from a struct iiLineFile with using create_Bip2(struct iiLineFile) function.
 * (BTW, the origin iiLineFile is getting from a file which contain two int integer in an line, 
 * using create_iiLineFile(char *filename) function)
 *
 * struct Bip2 contains only half of detail information of the bipartite. 
 * (I mean one struct Bip2 can store a iiLineFile completely, but it doesn't store detail information.)
 * you need two struct Bip2 to store all detail information of a Bipartite.
 * one is indexed by i1.(user)
 * one is indexed by i2.(item)
 *
 * struct L_Bip2 contains the result fo all kinds of recommendation algorithm.
 *
 */
#ifndef BIP2_H
#define BIP2_H

#include "../linefile/iilinefile.h"
#include "../linefile/iidlinefile.h"
#include "../compact/iidnet.h"


//this struct is just one way for describing Bipartite.
//other ways like (int ***xx) is also usefully.
//Bipartite contains two parts. e.g. user and item.
//but here, create_Bipartite only create user Bipartite or item Bipartite.
//if you want both, create two time with different i1toi2 arg.
struct Bip2 {
	int maxId;
	int minId;
	int idNum;
	long countMax;
	long countMin;
	long *count;
	int **id;
	long edgesNum;
};

struct L_Bip2 {
	double R;
	double PL;
	double HL;
    double IL;
    double NL;
	int *topL;
	int L;
};



//if i1toi2 is not zero, means the  column 1 is the index, and column 2 is the data saved into (int **id).
//column 1 and column 2 is the data in origin file.
//if i1toi2 is zero, means column 2 is the index.
struct Bip2 *create_Bip2(const struct iiLineFile * const file, int i1toi2);
void free_Bip2(struct Bip2 *bip);

struct L_Bip2 *create_L_Bip2(void);
void clean_L_Bip2(struct L_Bip2 *lp);
void free_L_Bip2(struct L_Bip2 *bip);

//abstract_Bip2 will abstract one edge from each user or item and return.
//the arg bip which has been abstracted will be changed, all abstracted edges will be deleted in the arg bip.
struct iiLineFile *abstract_Bip2(struct Bip2 *bip);
struct iiLineFile *backtofile_Bip2(struct Bip2 *bip);
void cutcount_Bip2(struct Bip2 *bip, long count);

//recommend methods
struct L_Bip2 *probs_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim);
struct L_Bip2 *HNBI_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim, double theta);
struct L_Bip2 *RENBI_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim, double eta);
struct L_Bip2 *heats_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim);
struct L_Bip2 *hybrid_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim, double lambda);
struct L_Bip2 *score_hybrid_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim, double lambda, double *score, double epsilon);

void *verifyBip2(struct Bip2 *bipi1, struct Bip2 *bipi2);

struct iiLineFile *divide_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, double rate);

void similarity_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, char *filename);
struct iidLineFile *similarity_realtime_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2);

#endif
