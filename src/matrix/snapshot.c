#include "../../inc/matrix/snapshot.h"
#include "../../inc/utility/sort.h"
#include "../../inc/utility/error.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


void free_Snapshot(struct Snapshot *net) {
	int i=0;
	for(i=0; i<net->maxId+1; ++i) {
		free(net->stat[i]);
	}
	free(net->stat);
}

struct Snapshot *create_Snapshot(const struct i3LineFile * const file) {
	int maxId=file->i1Max>file->i2Max?file->i1Max:file->i2Max;
	int minId=file->i1Min<file->i2Min?file->i1Min:file->i2Min;
	long linesNum=file->linesNum;
	struct i3Line *lines=file->lines;

	long i;
	char **stat=malloc((maxId+1)*sizeof(void *));
	assert(stat!=NULL);

	for(i=0; i<maxId+1; ++i) {
		stat[i]=malloc((maxId+1)*sizeof(char));
		assert(stat[i]!=NULL);
	}

	int x=0, y=0;
	for(i=0; i<linesNum; ++i) {
		int i1 =lines[i].i1;
		int i2 =lines[i].i2;
		while (x < i1) {
			for (; y<maxId+1; ++y) {
				stat[x][y] = 0;	
			}
			++x;
			y=0;
		}
		while (y < i2) {
			for (; y<i2; ++y) {
				stat[x][y] = 0;
			}
		}
		stat[x][y] = lines[i].i3;
		++y;
	}

	struct Snapshot *net = malloc(sizeof(struct Snapshot));
	assert(net != NULL);
	net->maxId=maxId;
	net->minId=minId;
	net->stat=stat;
	printf("Read Snapshot Successfully.\n"); fflush(stdout);
	return net;
}

void print_Snapshot(struct Snapshot *net, char *filename) {
	FILE *fp = fopen(filename, "w");
	fileError(fp, "print_Snapshot");
	int i, j;
	for (i=0; i<net->maxId + 1; ++i) {
		for (j=0; j< net->maxId + 1; ++j) {
			if (net->stat[i][j] != 0) {
				fprintf(fp, "%d,%d,%d\n", i, j, net->stat[i][j]);
			}
		}
	}
	fclose(fp);
	printf("print_Snapshot%s done.\n", filename);fflush(stdout);
}
