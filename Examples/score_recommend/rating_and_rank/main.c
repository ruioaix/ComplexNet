/**
 * ./run data/movielens/movielens_3c 5 0.76 0.84 0.78 0.15 25
 * ./run data/netflix/netflix_3c 5 0.78 0.84 0.78 0.17 25
 */

//#define NDEBUG  //for assert
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "inc/linefile/i3linefile.h"
#include "inc/compact/bip3i.h"
#include "inc/utility/random.h"
#include "inc/utility/error.h"

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	char *netfilename;
	int maxscore, stepNum;
	double theta, eta, epsilon, lambda;

	if (argc == 1) {
		netfilename = "data/movielens/movielens_3c";
		maxscore = 5;
		theta = 0.76; //score
		eta = 0.84; //degree
		epsilon = 0.78; //third
		lambda = 0.15; //hybrid
		stepNum = 25;
	}
	else if (argc == 8) {
		netfilename = argv[1];
		char *pEnd;
		maxscore = strtol(argv[2], &pEnd, 10);
		theta = strtod(argv[3], &pEnd);
		eta = strtod(argv[4], &pEnd);
		epsilon = strtod(argv[5], &pEnd);
		lambda = strtod(argv[6], &pEnd);
		stepNum = strtol(argv[7], &pEnd, 10);
	}
	else {
		printf("wrong argc\n");
		return 0;
	}

	//main
	struct i3LineFile *netfile = create_i3LineFile(netfilename);
	struct Bip3i *neti1 = create_Bip3i(netfile, 1);
	struct Bip3i *neti2 = create_Bip3i(netfile, 0);
	free_i3LineFile(netfile);

	int i,j;
	double *score = calloc(neti2->maxId + 1, sizeof(double));
	assert(score != NULL);
	for (i=0; i<neti2->maxId + 1; ++i) {
		if (neti2->count[i]) {
			for (j=0; j<neti2->count[i]; ++j) {
				score[i] += neti2->i3[i][j];
			}
			score[i] /= neti2->count[i];
		}
	}

	double *rankA_s = s_mass_rank_Bip3i(neti1, neti2, theta, maxscore);
	double *rankA_d = d_mass_rank_Bip3i(neti1, neti2, eta);
	double *rankA_t = thirdstepSD_mass_rank_Bip3i(neti1, neti2, epsilon);
	double *rankA_h = hybrid_rank_Bip3i(neti1, neti2, lambda);
	double *rankA_m = mass_rank_Bip3i(neti1, neti2);

	FILE *fp = fopen("sdth_rank", "w");
	fileError(fp, "main");
	for (i=0; i<neti2->maxId + 1; ++i) {
		if (neti2->count[i]) {
			fprintf(fp, "%d, %.17f, %.17f, %.17f, %.17f, %.17f, %.17f\n", i, score[i], rankA_s[i], rankA_d[i], rankA_t[i], rankA_h[i], rankA_m[i]);
		}
	}
	fclose(fp);

	double *s_rankA	= calloc(stepNum, sizeof(double));
	double *d_rankA	= calloc(stepNum, sizeof(double));
	double *t_rankA	= calloc(stepNum, sizeof(double));
	double *h_rankA	= calloc(stepNum, sizeof(double));
	double *m_rankA	= calloc(stepNum, sizeof(double));
	assert(s_rankA != NULL && d_rankA != NULL && t_rankA != NULL && h_rankA != NULL && m_rankA != NULL);

	int *s = calloc(stepNum, sizeof(int));
	for (i=0; i<neti2->maxId + 1; ++i) {
		if (neti2->count[i]) {
			//int r = 1;
			int r = floor((stepNum-1)*score[i]/5);
			++s[r];
			s_rankA[r] += rankA_s[i];
			d_rankA[r] += rankA_d[i];
			t_rankA[r] += rankA_t[i];
			h_rankA[r] += rankA_h[i];
			m_rankA[r] += rankA_m[i];
		}
	}
	free_Bip3i(neti1);
	free_Bip3i(neti2);



	fp = fopen("sdth_rank_m", "w");
	fileError(fp, "main");
	for (i=0; i<stepNum; ++i) {
		if (s[i]) {
			fprintf(fp, "%d, %.17f, %.17f, %.17f, %.17f, %.17f, %.17f\n", i, 5.0*((double)i)/stepNum, s_rankA[i]/s[i], d_rankA[i]/s[i], t_rankA[i]/s[i], h_rankA[i]/s[i], m_rankA[i]/s[i]);
		}
	}
	fclose(fp);

	free(rankA_s);
	free(rankA_d);
	free(rankA_t);
	free(rankA_h);
	free(rankA_m);
	free(s_rankA);
	free(d_rankA);
	free(t_rankA);
	free(h_rankA);
	free(m_rankA);
	free(score);
	free(s);

	//printf end time;
	t=time(NULL); printf("%s\n", ctime(&t)); fflush(stdout);
	return 0;
}
