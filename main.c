//#define NDEBUG
#include <stdio.h>
#include <time.h>
#include "inc/complexnet_file.h" //for readFileLBL;
#include "inc/complexnet_dnet.h" //for buildDNet;
#include "inc/complexnet_hashtable.h" //for buildDNet;

int main(int argc, char **argv)
{
	//printf begin time;
	time_t t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);

	//argv,argc process;
	char *datafilename="data/data1";
	char *originfilename="data/origin1";
	if (argc==2) datafilename=argv[1];
	else if (argc==3) {
		datafilename=argv[1];
		originfilename=argv[2];
	}
	else if (argc>3) {
		isError("two much args.\n");
	}
	

	//read data file, create net;
	struct NetFile *file=readNetFile(datafilename);
	struct HashTable *ht = createHashTable(file->maxId/2);
	edtype i;
	for (i=0; i<file->linesNum; ++i) {
		insertHEtoHT(ht, file->lines[i].vt1Id);
		insertHEtoHT(ht, file->lines[i].vt2Id);
	}
	FILE *fp = fopen("1", "w");
	fprintf(fp, "num: %d\n", ht->elementNum[ht->length-1]);
	elementNumSumHT(ht);
	fprintf(fp, "num: %d\n", ht->elementNum[ht->length-1]);
	elementNumBackHT(ht);
	fprintf(fp, "length: %d\n", ht->length);
	fprintf(fp, "num: %d\n", ht->elementNum[ht->length-1]);
	for (i=0; i<ht->length; ++i) {
		fprintf(fp, "%d: %d:\t", i, ht->elementNum[i]);
		int j;
		struct HashElement *he = ht->he[i];
		while(he) {
			fprintf(fp, "%ld,", he->element);
			he=he->next;
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	freeHashTable(ht);



	struct DirectNet *dnet=buildDNet(file);

	//read origin file, create IS;
	struct InfectSourceFile *ISs=readAllISfromFile(originfilename);

	//ISs, NET, infectRate, touchparam, loopNum, Thread_max.
	dnet_spread(ISs, dnet, 0.9, 0, 2, 4);

	//free;
	//free(IS->vt);
	freeISFile(ISs);
	freeNetFile(file);
	freeDNet(dnet);
	//printf end time;
	t=time(NULL); printf("%s", ctime(&t)); fflush(stdout);
	return 0;
}
