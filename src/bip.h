/**
 * recommandation is carried on between user and item, always two part.
 * maybe a biparte can contain the third part, like score that a user give to a item. but the third part (even another forth part) can be treated as a param.
 * this is more flexible.
 *
 * for bip:
 * 		the origin bipartite is coming from a struct LineFile with using create_Bip(struct LineFile) function.
 * 		(BTW, the origin LineFile is getting from a file which contain at least two int integer in an line, 
 * 		to a line containing three or more integers, only fetch two.  using create_LineFile(char *filename, 1, 1, -1))
 *
 * 		struct Bip contains only half information of the bipartite. 
 * 		(I mean one struct Bip can store a LineFile completely, but it doesn't store detail information.)
 * 		you need two struct Bip to store all detail information of a Bipartite.
 * 		one is indexed by i1(user).
 * 		one is indexed by i2(item).
 *
 * struct Metrics_Bip contains the result(all metrics) for all kinds of recommendation algorithm.
 *
 */
#ifndef CN_BIP_H
#define CN_BIP_H

#include "linefile.h"
#include "iidnet.h"

//this struct is just one way for describing Bipartite.
//other ways like (int ***xx) is also usefully.
//Bipartite contains two parts. e.g. user and item.
//but here, create_Bipartite only create user Bipartite or item Bipartite.
//if you want both, create two time with different index arg.
struct iiBip {
	int maxId;
	int minId;
	int idNum;
	long countMax;
	long countMin;
	long *count;
	int **edges;
	long edgesNum;
};

//if index is 1, means the i1 is the index, and i2 is the data saved into (int **edges).
//i1 and i2 is the data in LineFile.
//if index is 2, means i2 is the index.
struct iiBip *create_iiBip(const struct LineFile * const file, int index);
void free_iiBip(struct iiBip *bip);
struct iiBip * clone_iiBip(struct iiBip *bip);
void *verify_iiBip(struct iiBip *bipi1, struct iiBip *bipi2);

//the dividation will guarantee that: 
//	for each available user(degree of this user is at least one), at least there will be an edge in big part.
//	for each available item(degree of this item is at least one), at least there will be an edge in big part.
//so maybe some users are not existed in the small part.
void divide_iiBip(struct iiBip *bipi1, struct iiBip *bipi2, double rate, struct LineFile **small_part, struct LineFile **big_part);

//if target == 1, then calculate i1(mostly could be user)'s similarity.
//if target == 2, then calculate i2(mostly could be item)'s similarity.
struct LineFile *similarity_iiBip(struct iiBip *bipi1, struct iiBip *bipi2, int target);
struct LineFile *mass_similarity_iiBip(struct iiBip *bipi1, struct iiBip *bipi2);


struct Metrics_iiBip {
	double R;
	double PL;
	double HL;
    double IL;
    double NL;
	int *topL;
	int L;
};
struct Metrics_iiBip *create_MetricsiiBip(void);
void clean_MetricsiiBip(struct Metrics_iiBip *m);
void free_MetricsiiBip(struct Metrics_iiBip *m);

//recommend methods

struct Metrics_iiBip *mass_iiBip(struct iiBip *traini1, struct iiBip *traini2, struct iiBip *testi1, struct iiBip *testi2, struct iidNet *trainSim);

struct Metrics_iiBip *heats_iiBip(struct iiBip *traini1, struct iiBip *traini2, struct iiBip *testi1, struct iiBip *testi2, struct iidNet *trainSim);

struct Metrics_iiBip *HNBI_iiBip(struct iiBip *traini1, struct iiBip *traini2, struct iiBip *testi1, struct iiBip *testi2, struct iidNet *trainSim, double HNBI_param);

struct Metrics_iiBip *RENBI_iiBip(struct iiBip *traini1, struct iiBip *traini2, struct iiBip *testi1, struct iiBip *testi2, struct iidNet *trainSim, double RENBI_param);

struct Metrics_iiBip *hybrid_iiBip(struct iiBip *traini1, struct iiBip *traini2, struct iiBip *testi1, struct iiBip *testi2, struct iidNet *trainSim, double hybrid_param);

int *mass_getBK_iiBip(struct iiBip *traini1, struct iiBip *traini2, struct iiBip *testi1, struct iiBip *testi2, struct iidNet *userSim, double rate);

struct Metrics_iiBip *mass_topk_iiBip(struct iiBip *traini1, struct iiBip *traini2, struct iiBip *testi1, struct iiBip *testi2, struct iidNet *userSim, struct iidNet *itemSim, int topk);
struct Metrics_iiBip *mass_hs_iiBip(struct iiBip *traini1, struct iiBip *traini2, struct iiBip *testi1, struct iiBip *testi2, struct iidNet *userSim, struct iidNet *itemSim, int topk);

#endif
