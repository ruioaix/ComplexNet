#ifndef NET_H
#define NET_H

#include "file.h"
#include "sort.h"
#include "crosslist.h"

enum isDirect{ undirected=0, directed=1};

struct Net{
	enum isDirect isD;
	int vtNum;
	int *count;
	char *vtStat;

	struct crosslist *cl;
};

struct Net *createNet(struct LineInfo *li, long edNum, int vtNum, enum isDirect isD);
void addLine2Net(struct crosslist *cl, int x, int y, int time);
void freeNet(struct Net *net);

//S:1:clean, I:2:infect, R:3:dead
int isCleanVT(struct Net *net, int vtid);
int isInfectVT(struct Net *net, int vtid);
int isDeadVT(struct Net *net, int vtid);
void setVTclean(struct Net *net, int vtid);
void setVTinfect(struct Net *net, int vtid);
void setVTdead(struct Net *net, int vtid);

#endif
