#ifndef BIP3I_H
#define BIP3I_H

#include "../linefile/i3linefile.h"
#include "../compact/iidnet.h"
#include "../linefile/iidlinefile.h"


//this struct is just one way for describing Bipartite.
//other ways like (int ***xx) is also usefully.
//Bipartite contains two parts. e.g. user and item.
//but here, create_Bipartite only create user Bipartite or item Bipartite.
//if you want both, create two time with different i1toi2 arg.
struct Bip3i {
	int maxId;
	int minId;
	int idNum;
	long countMax;
	long countMin;
	long *count;
	int **id;
	int **i3;
	long edgesNum;
};

struct L_Bip3i {
	double R;
	double PL;
	double HL;
    double IL;
    double NL;
	int *topL;
	int L;
};

struct L_Bip3i *create_L_Bip3i(void);
void clean_L_Bip3i(struct L_Bip3i *lp);
void free_L_Bip3i(struct L_Bip3i *bip);

void free_Bip3i(struct Bip3i *bip);
//if i1toi2 is not zero, means the  column 1 is the index, and column 2 is the data saved into (int **id).
//column 1 and column 2 is the data in origin file.
//if i1toi2 is zero, means column 2 is the index.
struct Bip3i *create_Bip3i(const struct i3LineFile * const file, int i1toi2);

void *verifyBip3i(struct Bip3i *bipi1, struct Bip3i *bipi2);

struct i3LineFile *divide_Bip3i(struct Bip3i *bipi1, struct Bip3i *bipi2, double rate);

struct L_Bip3i *s_mass_Bip3i(struct Bip3i *traini1, struct Bip3i *traini2, struct Bip3i *testi1, struct Bip3i *testi2, struct iidNet *trainSim, double theta);
struct L_Bip3i *d_mass_Bip3i(struct Bip3i *traini1, struct Bip3i *traini2, struct Bip3i *testi1, struct Bip3i *testi2, struct iidNet *trainSim, double eta);
struct L_Bip3i *thirdstepSD_mass_Bip3i(struct Bip3i *traini1, struct Bip3i *traini2, struct Bip3i *testi1, struct Bip3i *testi2, struct iidNet *trainSim, double epsilon);

struct iidLineFile *similarity_realtime_Bip3i(struct Bip3i *bipi1, struct Bip3i *bipi2);
#endif
