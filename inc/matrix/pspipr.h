#ifndef PSPIPR_H
#define PSPIPR_H

#include "../linefile/iid3linefile.h"

struct PSPIPR{
	int maxId;
	int minId;
	double ***psir;
};
void free_PSPIPR(struct PSPIPR *net);
struct PSPIPR *create_PSPIPR(const struct iid3LineFile * const file);
void print_PSPIPR(struct PSPIPR *net, char *filename);

#endif
