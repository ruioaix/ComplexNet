#include "common.h"
#include "generatenet.h"
#include "iinet.h"
#include "error.h"
#include "sort.h"
#include "mt_random.h"
#include "iinetd.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>


static void insert_link_to_lf(int *id1, int *id2, int sumnline, struct iiLineFile *lf) {
	int i;
	for (i=0; i<sumnline; ++i) {
		lf->lines[lf->linesNum].i1 = id1[i];
		lf->lines[lf->linesNum].i2 = id2[i];
		lf->i1Max = lf->i1Max > id1[i]?lf->i1Max:id1[i];
		lf->i1Min = lf->i1Min < id1[i]?lf->i1Min:id1[i];
		lf->i2Max = lf->i2Max > id2[i]?lf->i2Max:id2[i];
		lf->i2Min = lf->i2Min < id2[i]?lf->i2Min:id2[i];
		lf->linesNum++;
	}
}

int main (int argc, char **argv) {
	print_time();
	//set_RandomSeed();

	int L;
	double alpha;
	if (argc == 3) {
		char *p;
		L = strtol(argv[1], &p, 10);
		alpha = strtod(argv[2], &p);
	}
	else if (argc == 1) {
		L = 512;
		alpha = 2;
	}
	else {
		isError("wrong args");
	}

	enum CICLENET cc = cycle;
	enum DIRECTNET dd = direct; 
	struct iiLineFile *file = generateNet_2D(L, cc, dd);
	struct iiNetD *net = create_iiNetD(file);

	int *id1 = malloc(L*L*sizeof(int));
	int *id2 = malloc(L*L*sizeof(int));
	int *hash = calloc((net->maxId + 1)*3, sizeof(int));
	int idNum = 0;

	int badluck = 0;
	long totalL = 0;
	long limit = (long)L*L;
	int k = 0;
	while (totalL < limit) {
		int i1 = genrand_int31()%(net->maxId + 1);
		int i2 = genrand_int31()%(net->maxId + 1);

		int sp = shortestpath_11_iiNetD(net, i1, i2);
		if (sp == -1) {
			isError("xx");
		}
		double p = pow(sp, 0-alpha);
		double rand = genrand_real3();
		if (rand > p) {
			continue;
		}

		int min = i1 < i2 ? i1 : i2;
		int max = i1 > i2 ? i1 : i2;
		if (hash[min + 2*max]) {
			//printf("not lucky, drop on same positon. try again.\n");
			badluck ++;
			continue;
		}
		hash[min + 2*max] = 1;
		id1[idNum] = i1;
		id2[idNum] = i2;
		++idNum;
		totalL += sp;
		printf("%ld\t%ld\r", totalL, limit);fflush(stdout);
	}
	free(hash);
	printf("badluck: %d, NumofAddedLinks: %d\n", badluck, idNum);

	long newLen = file->linesNum + idNum;
	struct iiLine * tmp = realloc(file->lines, (newLen)*sizeof(struct iiLine));
	if (tmp != NULL) {
		file->lines = tmp;
	}
	else {
		isError("very bad luck.");
	}
	insert_link_to_lf(id1, id2, idNum, file);
	//print_iiLineFile(file, "iilf");

	free(id1);
	free(id2);


	free_iiNetD(net);
	net = create_iiNetD(file);
	//verify_iiNet(net);
	//return 0;
	int *dis = get_ALLSP_iiNetD(net);

	double aveSP = 0;
	long spNum = 0;
	int i;
	for (i=0; i<net->maxId + 1; ++i) {
		if (dis[i]) {
			aveSP += (double)dis[i]*i;
			spNum += dis[i];
			//printf("%d\t%d\n", i, dis[i]);
		}
	}
	printf("\nspNum : %ld\n", spNum);
	aveSP /= spNum;

	printf("\nresult: %d\t%f\t%.9f\n", L, alpha, aveSP);


	free(dis);
	//free(choose);
	free_iiNetD(net);
	free_iiLineFile(file);
	print_time();
	return 0;
}
