
#include "../../inc/utility/statistics.h"
#include "stdlib.h"
#include "assert.h"
#include "string.h"
#include "math.h"
#include "stdio.h"

double *distrib_01(double *data, int length) {

	double *retn = malloc(100*sizeof(double));
	assert(retn != NULL);

	int dist[100];
	memset(dist, 0, 100*sizeof(int));
	
	int i;
	for (i=0; i<length; ++i) {
		if (data[i] >= 0 && data[i] <= 1) {
			int index = floor(data[i]*100);
			++dist[index];
		}
	}
	for (i=0; i<100; ++i) {
		retn[i] = dist[i]/(double)length;
		printf("%f, %f\n", i/100.0, retn[i]);
	}
	
	return retn;

}
