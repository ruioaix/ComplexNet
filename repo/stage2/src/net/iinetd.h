/**
 * some prerequisites before using this file.
 * 1,	minId must be 0.
 * 2,	idNum must > 1.
 * 3,	linesNum must > 0.
 * 4,	count[i] can be 0, if so, to[i] must be NULL.
 */

#ifndef CN_IINETD_H
#define CN_IINETD_H

#include "iilinefile.h"

struct iiNetD{
	int maxId;
	int minId;
	int idNum;
	long linesNum;
	long countMax;
	long countMin;
	long *count;
	int **to;
};

void free_iiNetD(struct iiNetD *net);
struct iiNetD *create_iiNetD(const struct iiLineFile * const file);

//thread routine, don't need arg, the routine will use dnet;
void verify_iiNetD(struct iiNetD *arg);

int shortestpath_11_iiNetD(struct iiNetD *net, int i1, int i2);
int *shortestpath_1A_iiNetD(struct iiNetD *net, int originId);
int *shortestpath_1A_S_iiNetD(struct iiNetD *net, int originId, int step, int *Num);
int *get_ALLSP_iiNetD(struct iiNetD *net);
#endif
