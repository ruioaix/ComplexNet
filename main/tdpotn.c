#include "generatenet.h"
#include "iinet.h"
#include "error.h"
#include "sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_time(void) {
	time_t t=time(NULL); 
	printf("%s", ctime(&t)); 
	fflush(stdout);
}


int main (int argc, char **argv) {
	print_time();

	int L = 300;
	enum CICLENET cc = non_cycle;
	struct iiLineFile *file = generateNet_2D(L, cc);
	//struct iiLineFile *file = generateNet_1D(L, cc);

	struct iiNet *net = create_iiNet(file);
	int *sp = shortestpath_1A_iiNet(net, 0);
	int *tmp = calloc((net->maxId + 1), sizeof(int));
	int j=0;
	int i;
	for (i=0; i<net->maxId + 1; ++i) {
		if (sp[i] > 0) {
			tmp[sp[i]] = 1;
		}
	}
	int *spv1 = calloc((net->maxId + 1), sizeof(int));
	j=0;
	for (i=0; i<net->maxId + 1; ++i) {
		if (tmp[i]) {
			spv1[j++] = i;	
		}
	}
	int *dis = get_ALLSP_iiNet(net);
	int *spv2 = calloc((net->maxId + 1), sizeof(int));
	j=0;
	for (i=0; i<net->maxId + 1; ++i) {
		if (dis[i]) {
			spv2[j++] = i;
		}
	}

	qsort_i_desc(spv1, 0, net->maxId);
	qsort_i_desc(spv2, 0, net->maxId);

	FILE *fp = fopen("output", "w");
	for(i=0; i<net->maxId + 1; ++i) {
		if (spv1[i] != spv2[i]) {
			fprintf(fp, "%d\t%d\n", spv1[i], spv2[i]);
			printf("%d\t%d\n", spv1[i], spv2[i]);
		}
	}
	

	free(sp);
	free(dis);
	free_iiNet(net);
	free_iiLineFile(file);
	print_time();
	return 0;
}
