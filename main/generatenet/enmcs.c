#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "mt_random.h"
#include "iilinefile.h"
#include "iinet.h"

int INITVALUE = 100000;
int STEP = 10000;

void print_time(void) {
	time_t t=time(NULL); 
	printf("%s\n", ctime(&t)); 
	fflush(stdout);
}

struct community {
	int idNum;
	int *id;
	int end;
};

struct community *generate_initNet(int N, int m0, char *initNetName) {
	FILE *fp = fopen(initNetName, "w");
	fileError(fp, initNetName);

	struct community *comN = malloc(N*sizeof(struct community));

	int i,j,k;
	for (i=0; i<N; ++i) {
		comN[i].id = malloc(INITVALUE * sizeof(int));
		comN[i].idNum = m0;
		comN[i].end = INITVALUE;
		int begin = m0*i;
		int end = m0*(i+1);
		for (j=begin; j<end; ++j) {
			comN[i].id[j-begin] = j;
			for (k=j+1; k<end; ++k) {
				fprintf(fp,"%d\t%d\n", j, k);
			}
		}
	}
	fclose(fp);
	return comN;
}

int *generate_AN(int N, int max, char *ANName) {
	FILE *fp = fopen(ANName, "w");
	fileError(fp, ANName);

	int *AN = malloc(N * sizeof(int));

	int i;
	for (i=0; i<N; ++i) {
		int aa = (int)(genrand_int31()%max + 1);
		fprintf(fp, "%d\n", aa);
		AN[i] = aa;
	}
	fclose(fp);
	return AN;
}

double *get_aveDegreeN(int N, int *netdegree, struct community *comN) {
	double *aveDegreeN = malloc(N*sizeof(double));
	int i,j;
	for (i=0; i<N; ++i) {
		int sumDegree = 0;
		for (j=0; j<comN[i].idNum; ++j) {
			int id = comN[i].id[j];
			sumDegree += netdegree[id];
		}
		aveDegreeN[i] = (double)sumDegree/comN[i].idNum;
	}
	return aveDegreeN;
}

int *set_chooseN(int k, int N, double *aveDegreeN, int *accuracy) {
	int *chooseN = malloc((*accuracy)*sizeof(int));
	int i,j;
	double sumAveDegreeN = 0;
	for (i=0; i<N; ++i) {
		if (i!=k) {
			sumAveDegreeN += aveDegreeN[i];
		}
	}
	int begin;
	int end = 0;
	for (i=0; i<N; ++i) {
		if (i!=k) {
			begin = end;
			end += (int)(aveDegreeN[i]/sumAveDegreeN*(*accuracy));
			//if (i==N-1 && end < accuracy) {
			//	printf("%d\t%d\t%d\n", begin, end, accuracy);
			//	isError("xx");
			//}
			for (j=begin; j<end; ++j) {
				chooseN[j] = i;
			}
		}
	}

	*accuracy = end;

	return chooseN;
}

struct distribN {
	int *id;
	int *nline;
	int idNum;
	int begin;
	int end;
};

void set_distribN(int N, int M, int L, int *AN, struct distribN *distribN, double *aveDegreeN, int maxId) {
	int i,j;
	for (i=0; i<N; ++i) {
		distribN[i].idNum = 0;
	}
	int accuracy = (N-1)*1000;
	for (i=0; i<N; ++i) {
		//get the distrubtion of the average degree of the other communities(N-1).
		int *chooseN = set_chooseN(i, N, aveDegreeN, &accuracy);
		distribN[i].begin = distribN[i].idNum;
		for (j=0; j<AN[i]; ++j) {
			++maxId;
			distribN[i].id[distribN[i].idNum] = maxId;
			distribN[i].nline[distribN[i].idNum] = L;
			++distribN[i].idNum;
			//random choose another community.
			int Nid = chooseN[genrand_int31()%accuracy];
			distribN[Nid].id[distribN[Nid].idNum] = maxId;
			distribN[Nid].nline[distribN[Nid].idNum] = M-L;
			++distribN[Nid].idNum;
		}
		distribN[i].end = distribN[i].idNum;
		free(chooseN);
	}
}


void set_choosem0(int i, struct community *comN, int *netdegree, int *choosem0, int *accuracy) {
	int *degree = malloc(comN[i].idNum*sizeof(int));
	int j,k;
	for (j=0; j<comN[i].idNum; ++j) {
		int id = comN[i].id[j];
		degree[j] = netdegree[id];
	}
	int sumDegree = 0;
	for (j=0; j<comN[i].idNum; ++j) {
		sumDegree += degree[j];
	}
	int begin;
	int end = 0;
	for (j=0; j<comN[i].idNum; ++j) {
		begin = end;
		end += (int)((double)degree[j]/sumDegree*(*accuracy));
		for (k=begin; k<end; ++k) {
			choosem0[k] = comN[i].id[j];
		}
	}
	*accuracy = end;
	free(degree);
}

void insert_id_to_comN(int i, int id, struct community *comN) {
	++comN[i].idNum;
	if (comN[i].idNum >= comN[i].end) {
		comN[i].end += STEP;
		int *tmp = realloc(comN[i].id, comN[i].end*sizeof(int));
		if (tmp != NULL) {
			comN[i].id = tmp;
		}
	}
	comN[i].id[comN[i].idNum-1] = id;
}

void insert_link_to_lf(int *id1, int *id2, int sumnline, struct iiLineFile *lf) {
	struct iiLine * tmp = realloc(lf->lines, (lf->linesNum+sumnline)*sizeof(struct iiLine));
	if (tmp != NULL) {
		lf->lines = tmp;	
	}
	else {
		isError("insert_link_to_lf");
	}
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

void add_links(int N, struct distribN *distribN, struct community *comN, int *netdegree, struct iiLineFile *lf) {
	int i,j;

	//choosem0 is the place holding the distribution of user's degree in each community.
	int comN_maxidNum = -1;
	for (i=0; i<N; ++i) {
		comN_maxidNum = comN_maxidNum > comN[i].idNum ? comN_maxidNum : comN[i].idNum;
	}
	int accuracy = comN_maxidNum * 1000;
	int *choosem0 = malloc(accuracy*sizeof(int));

	for (i=0; i<N; ++i) {
		int sumnline = 0;
		for (j=0; j<distribN[i].idNum; ++j) {
			sumnline += distribN[i].nline[j];	
		}
		int *id1 = malloc(sumnline * sizeof(int));
		int *id2 = malloc(sumnline * sizeof(int));
		//set choosem0
		set_choosem0(i, comN, netdegree, choosem0, &accuracy);
		//insert new user to comN.
		int s = 0;
		for (j=0; j<distribN[i].idNum; ++j) {
			int id = distribN[i].id[j];
			int ML = distribN[i].nline[j];
			if (j>=distribN[i].begin && j<distribN[i].end) {
				insert_id_to_comN(i, id, comN);
			}
			int k=0;
			for (k=0; k<ML; ++k) {
				id1[s] = id;
				id2[s] = choosem0[genrand_int31()%accuracy];
				++s;
			}
		}
		//insert new links to lf, new net will be generated from the new lf.
		//old net will be freed.
		insert_link_to_lf(id1, id2, sumnline, lf);
		free(id1);
		free(id2);
	}
	free(choosem0);
}


int *create_netdegree(struct iiLineFile *file) {

	int maxId=file->i1Max>file->i2Max?file->i1Max:file->i2Max;
	long linesNum=file->linesNum;

	int *count=calloc(maxId+1, sizeof(int));
	int i;
	for(i=0; i<linesNum; ++i) {
		++count[file->lines[i].i1];
		++count[file->lines[i].i2];
	}
	return count;
}

int main(int argc, char **argv)
{
	print_time();
	int N, m0, max, T, M, L;
	if (argc == 1) {
		N = 20;
		m0 = 100;
		max = 200;
		T = 20;
		M = 50;
		L = 10;
	}
	else if (argc == 7) {
		char *p;
		N = strtol(argv[1], &p, 10);
		m0 = strtol(argv[2], &p, 10);
		max = strtol(argv[3], &p, 10);
		T = strtol(argv[4], &p, 10);
		M = strtol(argv[5], &p, 10);
		L = strtol(argv[6], &p, 10);
	}
	else {
		isError("wrong args: ./generatenet-enmcs N m0 max T M L");
	}

	if (N<2 || m0 <2 || max <0 || T<1 || M<1 || M<L ) {
		isError("not proper args");
	}
	if (N>1 ) {
	};

	/************************************************************************************************/
	char *initNetName = "output_initNet";
	//comN is each community's information
	struct community *comN = generate_initNet(N, m0, initNetName);
	//netlf is the place holding all links.
	struct iiLineFile *netlf = create_iiLineFile(initNetName);

	char *ANName = "output_AN";
	//AN is a1,a2,...,ar.
	int *AN = generate_AN(N, max, ANName);
	//sumAN is the number of all new users.
	int sumAN=0;
	int i;
	for (i=0; i<N; ++i) {
		sumAN += AN[i];
	}
	//distribN is the place holding all new users and all new links' information.
	struct distribN *distribN = malloc(N*sizeof(struct distribN));
	for (i=0; i<N; ++i) {
		distribN[i].id = malloc(sumAN*sizeof(int));
		distribN[i].nline = malloc(sumAN*sizeof(int));
	}
	/************************************************************************************************/


	/************************************************************************************************/
	for (i=0; i<T; ++i) {
		//get all users' degree. 
		int *netdegree = create_netdegree(netlf);
		//get all community's average degree.
		double *aveDegreeN = get_aveDegreeN(N, netdegree, comN);
		//get information: the community each user will link to and the number of the links.
		int maxId=netlf->i1Max>netlf->i2Max?netlf->i1Max:netlf->i2Max;
		set_distribN(N, M, L, AN, distribN, aveDegreeN, maxId);
		//add links to netlf.
		add_links(N, distribN, comN, netdegree, netlf);

		free(netdegree);
		free(aveDegreeN);
	}
	/************************************************************************************************/

	//free_iiNet(net);
	free_iiLineFile(netlf);
	for (i=0; i<N; ++i) {
		free(comN[i].id);
		free(distribN[i].id);
		free(distribN[i].nline);
	}
	free(comN);
	free(distribN);
	free(AN);

	print_time();
	return 0;
}
