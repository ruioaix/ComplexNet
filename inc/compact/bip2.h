/**
 * bip2 means functions in this file only take care of the basic bipartite, 2 part.
 * e.g. user and item. no any other information, like the score which an user give to an item.
 *
 * the origin bipartite is coming from a struct iiLineFile with using create_Bip2(struct iiLineFile) function.
 * (BTW, the origin iiLineFile is getting from a file which contain two int integer in an line, 
 * to a line containing three or more integers, only fetch two.  using create_iiLineFile(char *filename) function)
 *
 * struct Bip2 contains only half information of the bipartite. 
 * (I mean one struct Bip2 can store a iiLineFile completely, but it doesn't store detail information.)
 * you need two struct Bip2 to store all detail information of a Bipartite.
 * one is indexed by i1.(user)
 * one is indexed by i2.(item)
 *
 * struct L_Bip contains the result fo all kinds of recommendation algorithm.
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

struct L_Bip {
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
//what divide_Bip2 function returns is a iiLineFile point, but the length of the memory which the point points is two struct iiLineFile.
//so, you need to use free_2_iiLineFile function instead of free_iiLineFile.
//divide Bip2 into two parts.
//return two struct iiLineFile. the first one is always the small one. the second is always the large one.
//the dividation will guarantee that: 
//	for each user which has at least one link, at least there will be a link in large divided part.
//	for each item which has at least one link, at least there will be a link in large divided part.
struct iiLineFile *divide_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, double rate);
//if i1ori2 == 1, then calculate i1(user)'s similarity.
//if i1ori2 == 0, then calculate i2(item)'s similarity.
struct iidLineFile *similarity_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, int i1ori2);

struct L_Bip *create_L_Bip(void);
void clean_L_Bip(struct L_Bip *lp);
void free_L_Bip(struct L_Bip *bip);

//recommend methods
struct L_Bip *test_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim);
struct L_Bip *probs_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim);
struct L_Bip *HNBI_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim, double theta);
struct L_Bip *RENBI_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim, double eta);
struct L_Bip *heats_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim);
struct L_Bip *hybrid_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim, double lambda);
struct L_Bip *score_hybrid_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim, double lambda, double *score, double epsilon);
struct L_Bip *onion_mass_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim, struct iidNet *userSim, double orate);
struct L_Bip *topR_probs_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim, struct iidNet *userSim, int topR);
struct L_Bip *probs_knn_Bip2(struct Bip2 *bipi1, struct Bip2 *bipi2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *trainSim, struct iidNet *userSim, int *bestK_R);


//void knn_getbest_Bip2(struct Bip2 *traini1, struct Bip2 *traini2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *userSim, int *bestK_R, int *bestK_PL);
void knn_getbest_Bip2(struct Bip2 *traini1, struct Bip2 *traini2, struct Bip2 *testi1, struct Bip2 *testi2, struct iidNet *userSim, int *bestK_R, int *bestK_PL, int userstep);


/************************************************************************************************************/
/****** the following functions are not used by me any more, but they works fine, so I will keep them. ******/
/************************************************************************************************************/

//abstract_Bip2 will abstract one edge from each user or item and return.
//the arg bip which has been abstracted will be changed, all abstracted edges will be deleted in the arg bip.
struct iiLineFile *abstract_Bip2(struct Bip2 *bip);
struct iiLineFile *backtofile_Bip2(struct Bip2 *bip);
void cutcount_Bip2(struct Bip2 *bip, long count);
void *verifyBip2(struct Bip2 *bipi1, struct Bip2 *bipi2);


#endif
