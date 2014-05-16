#include "iinet.h"
#include "base.h"
#include "mtprand.h"
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

void free_iiNet(struct iiNet *net) {
	print2l("%s =>> begin......\n", __func__);
	int i=0;
	for(i=0; i<net->maxId+1; ++i) {
		if (net->count[i]>0) {
			free(net->edges[i]);
		}
	}
	free(net->count);
	free(net->edges);
	free(net);
	print2l("%s =>> ......end.\n", __func__);
}

struct iiNet *create_iiNet(const struct LineFile * const lf) {
	print2l("%s =>> begin......\n", __func__);
	if (lf->i1 == NULL || lf->i2 == NULL || lf->linesNum == 0) {
		isError("%s =>> i1 or/and i2 is NULL, or/and linesNum is 0.\n", __func__);
	}
	int *i1 = lf->i1;
	int *i2 = lf->i2;
	long linesNum=lf->linesNum;
	long i;
	int maxId = i1[0];
	int minId = i1[0];
	for(i=0; i<linesNum; ++i) {
		maxId = maxId > i1[i] ? maxId : i1[i];	
		maxId = maxId > i2[i] ? maxId : i2[i];	
		minId = minId < i1[i] ? minId : i1[i];
		minId = minId < i2[i] ? minId : i2[i];
	}
	print3l("%s =>> get maxId: %d, minId: %d\n", __func__, maxId, minId);

	long *count=calloc(maxId+1, sizeof(long));
	assert(count!=NULL);

	for(i=0; i<linesNum; ++i) {
		++count[i1[i]];
		++count[i2[i]];
	}
	print3l("%s =>> get count.\n", __func__);

	int j;
	int idNum=0;
	for(j=0; j<maxId+1; ++j) {
		if (count[j]>0) {
			++idNum;
		}
	}
	print3l("%s =>> get idNum: %d.\n", __func__, idNum);

	int **edges=malloc((maxId+1)*sizeof(void *));
	assert(edges!=NULL);
	long countMax=0;
	long countMin=LONG_MAX;
	for(i=0; i<maxId+1; ++i) {
		countMax = countMax > count[i] ? countMax :count[i];
		countMin = countMin < count[i] ? countMin :count[i];
		if (count[i]!=0) {
			edges[i]=malloc(count[i]*sizeof(int));
			assert(edges[i]!=NULL);
		}
		else {
			edges[i] = NULL;
		}
	}
	print3l("%s =>> get countMax: %ld, countMin: %ld; and alloc memory for edges.\n", __func__, countMax, countMin);

	long *temp_count=calloc(maxId+1, sizeof(long));
	assert(temp_count!=NULL);

	for(i=0; i<linesNum; ++i) {
		int ii1 =i1[i];
		int ii2 =i2[i];
		edges[ii1][temp_count[ii1]++]=ii2;
		edges[ii2][temp_count[ii2]++]=ii1;
	}
	free(temp_count);
	print3l("%s =>> fill in edges\n", __func__);

	struct iiNet *net = malloc(sizeof(struct iiNet));
	assert(net != NULL);
	net->maxId=maxId;
	net->minId=minId;
	net->edgesNum=linesNum;
	net->idNum=idNum;
	net->countMax=countMax;
	net->countMin=countMin;
	net->count=count;
	net->edges=edges;
	print3l("%s =>> return iiNet.\n", __func__);
	print2l("%s =>> ......end.\n", __func__);
	return net;
}

void print_iiNet(struct iiNet *net, char *filename) {
	print2l("%s =>> begin......\n", __func__);
	FILE *fp = fopen(filename, "w");
	fileError(fp, "print_iiNet");
	int i;
	long j;
	for (i=0; i<net->maxId + 1; ++i) {
		if (net->count[i] > 0) {
			for (j=0; j<net->count[i]; ++j) {
				if (i < net->edges[i][j]) {
					fprintf(fp, "%d\t%d\n", i, net->edges[i][j]);
				}
			}
		}
	}
	fclose(fp);
	print2l("%s =>> ......end.\n", __func__);
}

int delete_node_iiNet(struct iiNet *net, int nid) {
	print2l("%s =>> begin......\n", __func__);
	long i, j;
	if (nid < 0 || nid > net->maxId || net->count[nid] == 0) {
		print2l("%s =>> ......end.\n", __func__);
		return 0;
	}
	net->edgesNum -= net->count[nid];
	for (i = 0; i < net->count[nid]; ++i) {
		int neigh = net->edges[nid][i];
		long count = net->count[neigh];
		for (j = 0; j < net->count[neigh]; ++j) {
			if (net->edges[neigh][j] == nid) {
				net->edges[neigh][j] = net->edges[neigh][--(net->count[neigh])];
				break;
			}
		}
		if (j == count) {
			isError("delete_node_iiNet net wrong");
		}
		if (net->count[neigh] == 0) {
			net->idNum--;
			free(net->edges[neigh]);
			net->edges[neigh] = NULL;
		}
	}
	free(net->edges[nid]);
	net->edges[nid] = NULL;
	//printf("delete node %d, node %d count is %ld, now the edgesNum is %ld\n", nid, nid, net->count[nid], net->edgesNum);
	net->count[nid] = 0;
	net->idNum--;
	print2l("%s =>> ......end.\n", __func__);
	return 1;
}

int delete_link_iiNet(struct iiNet *net, int x, int y) {
	print2l("%s =>> begin......\n", __func__);
	if (x > net->maxId || y > net->maxId || net->count[x] == 0 || net->count[y] == 0) {
		print2l("%s =>> ......end.\n", __func__);
		return 0;
	}
	long i;
	int find_x_in_y = 0, find_y_in_x = 0;
	for (i = 0; i < net->count[x]; ++i) {
		if (net->edges[x][i] == y) {
			net->edges[x][i] = net->edges[x][--(net->count[x])];
			find_y_in_x = 1;
			if (net->count[x] == 0) {
				free(net->edges[x]);
				net->edges[x] = NULL;
				net->idNum--;
			}
			break;
		}
	}
	for (i = 0; i < net->count[y]; ++i) {
		if (net->edges[y][i] == x) {
			net->edges[y][i] = net->edges[y][--(net->count[y])];
			find_x_in_y = 1;
			if (net->count[y] == 0) {
				free(net->edges[y]);
				net->edges[y] = NULL;
				net->idNum--;
			}
			break;
		}
	}
	int find = find_x_in_y + find_y_in_x;
	if (find == 0) {
		print2l("%s =>> ......end.\n", __func__);
		return 0;
	}
	else if (find == 2) {
		net->edgesNum--;
		print2l("%s =>> ......end.\n", __func__);
		return 1;
	}
	isError("%s =>> net has something wrong.\n", __func__);
	//can not get here.
	return -1;
}

long *degree_distribution_iiNet(struct iiNet *net) {
	print2l("%s =>> begin......\n", __func__);
	long *cd = calloc(net->countMax + 1, sizeof(long));
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		cd[net->count[i]]++;
	}
	print2l("%s =>> ......end.\n", __func__);
	return cd;
}

static int extract_backbone_iiNet(int nid, struct iiNet *net, char *fg, int *left, int *right) {
	print4l("%s =>> begin......\n", __func__);
	if (fg[nid] == 1) isError("%s =>> the %d node is in another backbone.\n", __func__, nid);
	int lN = 0, rN = 0;
	left[lN++] = nid;
	fg[nid] = 1;
	int conn = 1;
	int i;
	long j;
	while(lN) {
		rN = 0;
		for (i = 0; i < lN; ++i) {
			int id = left[i];
			for (j = 0; j < net->count[id]; ++j) {
				int neigh = net->edges[id][j];
				if (fg[neigh] == 0) {
					fg[neigh] = 1;
					++conn;
					right[rN++] = neigh;
				}
			}
		}
		int *tmp = left;
		left = right;
		right = tmp;
		lN = rN;
	}
	print4l("%s =>> ......end.\n", __func__);
	return conn;
}

//if net->count[nid] == 0, then I presume nid node is not existed.
int robust_iiNet(struct iiNet *net, int *robust) {
	print2l("%s =>> begin......\n", __func__);
	int N = net->idNum;
	int maxru = 0;
	int already = 0;
	int maxi = 0;

	char *fg = calloc(net->maxId + 1, sizeof(char));
	int *left = malloc((net->maxId + 1) * sizeof(int));
	int *right = malloc((net->maxId + 1) * sizeof(int));

	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		if (fg[i] == 0 && net->count[i]) {
			int conn = extract_backbone_iiNet(i, net, fg, left, right);
			already += conn;
			if (maxru < conn) {
				maxru = conn;
				maxi = i;
			}
			if (maxru >= N-already) break;
		}
	}

	free(fg);
	free(left);
	free(right);
	*robust = maxru;
	print2l("%s =>> ......end.\n", __func__);
	return maxi;
}

void verify_duplicatePairs_iiNet(struct iiNet *net) {
	print2l("%s =>> begin......\n", __func__);
	long i;
	int j,k;
	int *place = malloc((net->maxId+1)*sizeof(int));
	for (k=0; k<net->maxId + 1; ++k) {
		place[k] = -1;
	}
	long dpairsNum=0;
	for (j=0; j<net->maxId+1; ++j) {
		for (i=0; i < net->count[j]; ++i) {
			int neigh = net->edges[j][i];
			if (place[neigh] == -1) {
				place[neigh] = 1;
			}
			else {
				print1l("%s =>> duplicate pairs %ld:\t%d\t%d\n", __func__, ++dpairsNum, j, neigh);
			}
		}
		for (i = 0; i < net->count[j]; ++i) {
			int neigh = net->edges[j][i];
			place[neigh] = -1;
		}
	}
	free(place);
	if (dpairsNum) {
		isError("%s =>> net has some duplicate pairs.\n", __func__);
	}
	print2l("%s =>> ......end.\n", __func__);
	fflush(stdout);
}

void verify_fullyConnected_iiNet(struct iiNet *net) {
	print2l("%s =>> begin......\n", __func__);
	char *fg = calloc(net->maxId + 1, sizeof(char));
	int *left = malloc((net->maxId + 1) * sizeof(int));
	int *right = malloc((net->maxId + 1) * sizeof(int));
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		if (net->count[i]) break;
	}
	int conn = extract_backbone_iiNet(i, net, fg, left, right);
	free(fg);
	free(left);
	free(right);
	if (conn != net->idNum) {
		isError("%s =>> not connectedness.\n", __func__);
	}
	print2l("%s =>> ......end.\n", __func__);
}
