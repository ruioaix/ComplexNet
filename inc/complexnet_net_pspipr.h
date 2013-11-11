#ifndef COMPLEXNET_NET_PSPIPR_H
#define COMPLEXNET_NET_PSPIPR_H


#include "../inc/linefile/iid3linefile.h"

struct Net_PSPIPR{
	int maxId;
	int minId;
	double ***psir;
};
void free_Net_PSPIPR(void);
struct Net_PSPIPR * get_Net_PSPIPR(void);
void create_Net_PSPIPR(const struct iid3LineFile * const file);
void print_Net_PSPIPR(char *filename);
//double find_Net_PSPIPR(int infect_source, int eye, int status);

#endif
