/**
 * recommandation is carried on between user and item, always two part.
 * maybe a biparte can contain the third part, like score that a user give to a item. but the third part (even another forth part) can be treated as a param.
 * this is more flexible.
 * In the following part, bipii means there is only two part, two int; bipiid means there is the third part which is a double.
 * this is similar to xxlinefile, e.g. bipi3 means there is the third part which is a int.
 *
 * for bipii:
 * 		the origin bipartite is coming from a struct iiLineFile with using create_Bipii(struct iiLineFile) function.
 * 		(BTW, the origin iiLineFile is getting from a file which contain two int integer in an line, 
 * 		to a line containing three or more integers, only fetch two.  using create_iiLineFile(char *filename) function)
 *
 * 		struct Bipii contains only half information of the bipartite. 
 * 		(I mean one struct Bipii can store a iiLineFile completely, but it doesn't store detail information.)
 * 		you need two struct Bipii to store all detail information of a Bipartite.
 * 		one is indexed by i1.(user)
 * 		one is indexed by i2.(item)
 *
 * struct Metrics_Bipii contains the result(all metrics) fo all kinds of recommendation algorithm.
 *
 */
#ifndef CN_BIP_H
#define CN_BIP_H

#include "iilinefile.h"
#include "iidlinefile.h"
#include "iidnet.h"


/************************************************************************************************************/
/*******************************following is about Bipii*****************************************************/
/************************************************************************************************************/

//this struct is just one way for describing Bipartite.
//other ways like (int ***xx) is also usefully.
//Bipartite contains two parts. e.g. user and item.
//but here, create_Bipartite only create user Bipartite or item Bipartite.
//if you want both, create two time with different i1toi2 arg.
struct Bipii {
	int maxId;
	int minId;
	int idNum;
	long countMax;
	long countMin;
	long *count;
	int **id;
	long edgesNum;
};

//if i1toi2 is not zero, means the  column 1 is the index, and column 2 is the data saved into (int **id).
//column 1 and column 2 is the data in origin file.
//if i1toi2 is zero, means column 2 is the index.
struct Bipii *create_Bipii(const struct iiLineFile * const file, int i1toi2);
void free_Bipii(struct Bipii *bip);

//the dividation will guarantee that: 
//	for each available user(degree of this user is at least one), at least there will be a link in big part.
//	for each available item(degree of this item is at least one), at least there will be a link in big part.
//so maybe some users are not existed in the small part.
void divide_Bipii(struct Bipii *bipi1, struct Bipii *bipi2, double rate, struct iiLineFile **small_part, struct iiLineFile **big_part);

//if i1ori2 == 1, then calculate i1(user)'s similarity.
//if i1ori2 == 0, then calculate i2(item)'s similarity.
struct iidLineFile *similarity_Bipii(struct Bipii *bipi1, struct Bipii *bipi2, int i1ori2);


struct Metrics_Bipii {
	double R;
	double PL;
	double HL;
    double IL;
    double NL;
	int *topL;
	int L;
};
struct Metrics_Bipii *create_MetricsBipii(void);
void clean_MetricsBipii(struct Metrics_Bipii *lp);
void free_MetricsBipii(struct Metrics_Bipii *bip);

//recommend methods

struct Metrics_Bipii *mass_Bipii(struct Bipii *traini1, struct Bipii *traini2, struct Bipii *testi1, struct Bipii *testi2, struct iidNet *trainSim);
struct Metrics_Bipii *mass_topR_Bipii(struct Bipii *traini1, struct Bipii *traini2, struct Bipii *testi1, struct Bipii *testi2, struct iidNet *itemSim, struct iidNet *userSim, int mass_topR);
struct Metrics_Bipii *mass_topS_Bipii(struct Bipii *traini1, struct Bipii *traini2, struct Bipii *testi1, struct Bipii *testi2, struct iidNet *itemSim, struct iidNet *userSim, double mass_topS);
struct Metrics_Bipii *mass_corK_Bipii(struct Bipii *traini1, struct Bipii *traini2, struct Bipii *testi1, struct Bipii *testi2, struct iidNet *itemSim, struct iidNet *userSim, double mass_corK, double *mass_corK_itemAveDre, double mass_corK_itemAveDreMax, int mass_topR);

struct Metrics_Bipii *heats_Bipii(struct Bipii *traini1, struct Bipii *traini2, struct Bipii *testi1, struct Bipii *testi2, struct iidNet *trainSim);

struct Metrics_Bipii *HNBI_Bipii(struct Bipii *traini1, struct Bipii *traini2, struct Bipii *testi1, struct Bipii *testi2, struct iidNet *trainSim, double HNBI_param);

struct Metrics_Bipii *RENBI_Bipii(struct Bipii *traini1, struct Bipii *traini2, struct Bipii *testi1, struct Bipii *testi2, struct iidNet *trainSim, double RENBI_param);

struct Metrics_Bipii *hybrid_Bipii(struct Bipii *traini1, struct Bipii *traini2, struct Bipii *testi1, struct Bipii *testi2, struct iidNet *trainSim, double hybrid_param);

void experiment_knn_Bipii(struct Bipii *traini1, struct Bipii *traini2, struct Bipii *testi1, struct Bipii *testi2, struct iidNet *userSim);


#endif
