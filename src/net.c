#include "../inc/net.h"

void freeNet(struct Net *net)
{
	free_crosslist(net->cl);
	free(net->count);
	free(net->vtStat);
	free(net);
}

void addLine2Net(struct crosslist *cl, int x, int y, int time)
{
	int reallocStep=10;
	struct cl_element *pos;
	pos=getElementCL(cl, x, y);
	if(pos) {
		++pos->timeNum;
		if (pos->timeNum%reallocStep==0) {
			int *temp=realloc(pos->time, (pos->timeNum+reallocStep)*sizeof(int));
			memError(temp, "addTimetoElementCL temp");
			pos->time=temp;
		}
		pos->time[pos->timeNum-1]=time;
	}
	else {
		pos=calloc(1, sizeof(struct cl_element));
		memError(pos, "addTimetoElementCL pos");
		pos->x=x;
		pos->y=y;
		pos->timeNum=1;
		pos->time=malloc(reallocStep*sizeof(int));
		memError(pos->time, "addTimetoElementCL pos->time");
		pos->time[0]=time;
		insertElementCL(cl, pos);
	}
}

struct Net *createNet(struct LineInfo *li, long edNum, int vtNum, enum isDirect isD)
{
	//printf("build net: ");fflush(stdout);
	//allocate nessary memory for net;
	struct Net *net=calloc(1, sizeof(struct Net));	
	memError(net, "createNet net");

	net->count=calloc(vtNum, sizeof(int));
	memError(net->count, "createNet net->count");

	net->vtStat=malloc(vtNum*sizeof(char));
	memError(net->vtStat, "createNet net->vtStat");
	memset(net->vtStat, 1, vtNum*sizeof(char));

	net->cl=createCrossList(vtNum, vtNum);
	net->vtNum=vtNum;
	net->isD=isD;

	int i;
	for (i=0; i<edNum; i++) {
		if (i%100000==0) {
			printf("build net: %f\r", (double)i/(double)edNum); 
			fflush(stdout);
		}
		if (isD==undirected) {
			long vtx=li[i].vt1Id>li[i].vt2Id?li[i].vt1Id:li[i].vt2Id;
			long vty=li[i].vt1Id>li[i].vt2Id?li[i].vt2Id:li[i].vt1Id;
			addLine2Net(net->cl, vtx, vty, li[i].time);
		}
		else {
			addLine2Net(net->cl, li[i].vt1Id, li[i].vt2Id, li[i].time);
		}
	}

	printf("build net : finish.\n"); fflush(stdout);
	return net;
}

//S:1:clean, I:2:infect, R:3:dead
int isCleanVT(struct Net *net, int vtid)
{
	if (net->vtStat[vtid]==1)
		return 1;
	return 0;
}
int isInfectVT(struct Net *net, int vtid)
{
	if (net->vtStat[vtid]==2)
		return 1;
	return 0;
}
int isDeadVT(struct Net *net, int vtid)
{
	if (net->vtStat[vtid]==3)
		return 1;
	return 0;
}
void setVTclean(struct Net *net, int vtid)
{
	net->vtStat[vtid]=1;
}
void setVTinfect(struct Net *net, int vtid)
{
	net->vtStat[vtid]=2;
}
void setVTdead(struct Net *net, int vtid)
{
	net->vtStat[vtid]=3;
}

