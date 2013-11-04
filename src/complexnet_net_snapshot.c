#include "../inc/complexnet_net_snapshot.h"
#include "../inc/complexnet_sort.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


static struct Net_SNAPSHOT net;

struct Net_SNAPSHOT * get_Net_SNAPSHOT(void) {
	return &net;
}

void free_Net_SNAPSHOT(void) {
	int i=0;
	for(i=0; i<net.maxId+1; ++i) {
		free(net.stat[i]);
	}
	free(net.stat);
}

void create_Net_SNAPSHOT(const struct i3LineFile * const file) {
	int maxId=file->iMax;
	int minId=file->iMin;
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

	net.maxId=maxId;
	net.minId=minId;
	net.stat=stat;
	printf("Read Snapshot Successfully.\n"); fflush(stdout);
}

//find status net.status[i][eye].
//int find_Net_SNAPSHOT_status(infect_source, eye) {
//	long i;
//	int node;
//	for (i=0; i< net.count[infect_source]; ++i) {
//		node = net.edges[infect_source][i];
//		//eye is 8 , node = 1
//		if (eye > node) {
//			continue;		
//		}
//		else if (eye == node) {
//			return net.status[infect_source][i];
//		}
//		else {
//			return 0;
//		}
//	}
//	return 0;
//}