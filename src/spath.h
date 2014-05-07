#ifndef CN_SPATH_H
#define CN_SPATH_H

#include "iinet.h"
#include "iidnet.h"

/*
 * this is a shortest path algorithm for a special network.
 * net is a ii net.
 * air is a iid net.
 * air's nodes is a subset of net's nodes.
 * in net network, egdes don't have weight, or we can think all weight is 1.
 * but in air network, edges has weight and all weight is not less than 1.
 *
 * then get_kind01_SP will save the average shortest path into *avesp. 
 * because in this special situation all pairs in network is contected, so I
 * don't conside the possibility of containing non-reachable pairs in network.
 *
 * and get_kind02_SP will return all the node's shortest path to node "id".
 *
 */
void get_kind01_SP(struct iiNet *net, struct iidNet *air, double *avesp);
double *get_kind02_SP(struct iiNet *net, struct iidNet *air, int id);

//standard dijkstra algorithm.
double *dijkstra_1A_iidNet(struct iidNet *net, int nid);

//this spath01 is for unweighted and undirect shortest path.
int *spath01_1A_iiNet(struct iiNet *net, int originId);
int *spath01_step_1A_iiNet(struct iiNet *net, int originId, int step, int *Num);
void avesp_spath01_iiNet(struct iiNet *net, double *avesp);

//this spath02 is FW algorithm for unweighted and undirected net.
int **spath02_AA_iiNet(struct iiNet *net);

//this spath03 is for unweighted_undirected base and weighted_undirected air network.
//the weight of each link in air network is >= 1.0.
void avesp_spath03_Net(struct iiNet *net, struct iidNet *air, double *avesp);

//this spath04 is for unweighted and undirected net.
//to find how many shortest pathes use only base or only air or both.
void useRate_spath04_iiNet(struct iiNet *net, struct iiNet *air, double *useRate, double *sameRate, double *cleanRate, double *avesp);

//this spath05 is for unweighted and undirected net.
//to find coupling of two net: base and air.
void coupling_spath05_iiNet(struct iiNet *net, struct iiNet *air, double *coupling, double *avesp);

//this spath06 is for unweighted_undirected base and weighted_undirected XE network.
//to find gini of two net: base and XE.
void gini_spath06_Net(struct iiNet *base, struct iidNet *XE, double *avesp, double *gini);
#endif
