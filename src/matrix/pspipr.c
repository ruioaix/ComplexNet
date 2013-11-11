#include "../../inc/matrix/pspipr.h"
#include "../../inc/utility/sort.h"
#include "../../inc/utility/error.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

void free_PSPIPR(struct PSPIPR *net) {
	int i, j;
	for(i=0; i<net->maxId+1; ++i) {
		for (j=0; j<net->maxId+1; ++j) {
			free(net->psir[i][j]);
		}
		free(net->psir[i]);
	}
	free(net->psir);
}

//most of PSPIPR is 1,0,0. 
//so when PSPIPR is 100, I just set the address to NULL.
struct PSPIPR *create_PSPIPR(const struct iid3LineFile * const file) {
	int maxId=file->i1Max>file->i2Max?file->i1Max:file->i2Max;
	int minId=file->i1Min<file->i2Min?file->i1Min:file->i2Min;
	long linesNum=file->linesNum;
	struct iid3Line *lines=file->lines;

	double ***psir=malloc((maxId+1)*sizeof(void *));
	assert(psir!=NULL);

	int i;
	for(i=0; i<maxId+1; ++i) {
		psir[i]=malloc((maxId+1)*sizeof(void *));
		assert(psir[i]!=NULL);
	}

	int x=0, y=0;
	for(i=0; i<linesNum; ++i) {
		int i1 =lines[i].i1;
		int i2 =lines[i].i2;
		while (x < i1) {
			for (; y<maxId+1; ++y) {
				psir[x][y] = NULL;	
			}
			++x;
			y=0;
		}
		while (y < i2) {
			for (; y<i2; ++y) {
				psir[x][y] = NULL;
			}
		}
		psir[x][y] = malloc(3*sizeof(double));
		assert(psir[x][y] != NULL);
		psir[x][y][0] = lines[i].d3;
		psir[x][y][1] = lines[i].d4;
		psir[x][y][2] = lines[i].d5;
		++y;
	}

	struct PSPIPR *net = malloc(sizeof(struct PSPIPR));
	assert(net != NULL);
	net->maxId=maxId;
	net->minId=minId;
	net->psir=psir;
	printf("Read PSPIPR Successfully\n"); fflush(stdout);
	return net;
}

//I also don't output 100 PSPIPR.
void print_PSPIPR(struct PSPIPR *net, char *filename) {
	FILE *fp = fopen(filename, "w");
	fileError(fp, "print_Net_PSPIPR");
	int i, j;
	for (i=0; i<net->maxId + 1; ++i) {
		for (j=0; j< net->maxId + 1; ++j) {
			if (net->psir[i][j] != NULL) {
				fprintf(fp, "%d, %d, %0.17f, %0.17f, %0.17f\n", i, j, net->psir[i][j][0], net->psir[i][j][1], net->psir[i][j][2]);
			}
		}
	}
	fclose(fp);
	printf("print_Net_PSPIPR %s done.\n", filename);fflush(stdout);
}
