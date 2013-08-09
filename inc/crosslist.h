#ifndef CROSSLIST_H
#define CROSSLIST_H
#include "errorhandle.h"

struct cl_element{
	int x;
	int y;
	int timeNum;
	int *time;	
	struct cl_element *left;
	struct cl_element *right;
	struct cl_element *up;
	struct cl_element *down;
};

struct crosslist{
	int row_max;
	int col_max;
	struct cl_element **row;
	struct cl_element **col;
};
// this crosslist is for time complex network.
// the vtId doesn't need to be continuous from 0 to xx(for the current project with DuanBingCheng, xx is 3210678). 
// but row_max(on most status, col_max==row_max) shold larger than vtMaxId. 
// if vtId is not continuous, unnecessary memory usage is acceptable, only row and col contain some unnecessary memory usage.
// the best choice is the matrix, but 3210678*3210678 is too big, event use bit, still cost 100G space.
// the crosslist's memory usage is proportional to the num of edges. 
// in time complex network, one edge can contain different time, it's good choice for using crosslist.
struct crosslist *createCrossList(int rowLen, int colLen);
//if element already exist, exit with a message.
void insertElementCL(struct crosslist *cl, struct cl_element *cl_em);
//if element doesn't exist, nothing been done.
void deleteElementCL(struct crosslist *cl, int x, int y);
//getElementCL is get the actual element in crosslist, if it changes, cl changes.
struct cl_element *getElementCL(struct crosslist *cl, int x, int y);
//free
void free_cl_element(struct cl_element *cl_em);
void free_crosslist(struct crosslist *cl);

#endif
