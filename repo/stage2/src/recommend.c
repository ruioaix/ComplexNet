/**
 * mass recommendation
 */

//#define NDEBUG  //for assert
#include <stdio.h>

#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "iilinefile.h"
#include "iidlinefile.h"
#include "bip.h"
#include "error.h"
#include "mt_random.h"

static void print_time(void) {
	time_t t=time(NULL); 
	printf("%s", ctime(&t)); 
	fflush(stdout);
}

static void create_2dataset(const char * const netfilename, struct Bipii **traini1, struct Bipii **traini2, struct Bipii **testi1, struct Bipii **testi2) {
	struct iiLineFile *netfile = create_iiLineFile(netfilename);
	struct Bipii *neti1 = create_Bipii(netfile, 1);
	struct Bipii *neti2 = create_Bipii(netfile, 0);
	free_iiLineFile(netfile);
	struct iiLineFile *first, *second;
	divide_Bipii(neti1, neti2, 0.1, &first, &second);
	free_Bipii(neti1);
	free_Bipii(neti2);
	*traini1 = create_Bipii(second, 1);
	*traini2 = create_Bipii(second, 0);
	*testi1 = create_Bipii(first, 1);
	*testi2 = create_Bipii(first, 0);
	free_iiLineFile(first);
	free_iiLineFile(second);
}

static void get_ItemSimilarity(struct Bipii *traini1, struct Bipii *traini2, struct iidNet **itemSim) {
	struct iidLineFile *itemSimilarityfile = similarity_Bipii(traini1, traini2, 0);
	*itemSim = create_iidNet(itemSimilarityfile);
	free_iidLineFile(itemSimilarityfile);
}

int mass(const char * const netfilename) {
	print_time();
	struct Bipii *traini1, *traini2, *testi1, *testi2;
	create_2dataset(netfilename, &traini1, &traini2, &testi1, &testi2);

	struct iidNet *itemSim=NULL;
	get_ItemSimilarity(traini1, traini2, &itemSim);
	
	struct Metrics_Bipii *mass_result = mass_Bipii(traini1, traini2, testi1, testi2, itemSim);
	printf("mass\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", mass_result->R, mass_result->PL, mass_result->IL, mass_result->HL, mass_result->NL);
	free_MetricsBipii(mass_result);

	free_Bipii(traini1);
	free_Bipii(traini2);
	free_Bipii(testi1);
	free_Bipii(testi2);
	free_iidNet(itemSim);
	
	print_time();
	return 0;
}

int heats(const char * const netfilename) {
	print_time();
	struct Bipii *traini1, *traini2, *testi1, *testi2;
	create_2dataset(netfilename, &traini1, &traini2, &testi1, &testi2);

	struct iidNet *itemSim=NULL;
	get_ItemSimilarity(traini1, traini2, &itemSim);
	
	struct Metrics_Bipii *heats_result = heats_Bipii(traini1, traini2, testi1, testi2, itemSim);
	printf("heats\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", heats_result->R, heats_result->PL, heats_result->IL, heats_result->HL, heats_result->NL);
	free_MetricsBipii(heats_result);

	free_Bipii(traini1);
	free_Bipii(traini2);
	free_Bipii(testi1);
	free_Bipii(testi2);
	free_iidNet(itemSim);
	
	print_time();
	return 0;
}

int HNBI(const char * const netfilename, const char * const HNBI_arg) {
	print_time();
	char *p;
	double x=strtod(HNBI_arg, &p);
	struct Bipii *traini1, *traini2, *testi1, *testi2;
	create_2dataset(netfilename, &traini1, &traini2, &testi1, &testi2);

	struct iidNet *itemSim=NULL;
	get_ItemSimilarity(traini1, traini2, &itemSim);
	
	struct Metrics_Bipii *HNBI_result = HNBI_Bipii(traini1, traini2, testi1, testi2, itemSim, x);
	printf("HNBI\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", HNBI_result->R, HNBI_result->PL, HNBI_result->IL, HNBI_result->HL, HNBI_result->NL);
	free_MetricsBipii(HNBI_result);

	free_Bipii(traini1);
	free_Bipii(traini2);
	free_Bipii(testi1);
	free_Bipii(testi2);
	free_iidNet(itemSim);
	
	print_time();
	return 0;
}

int RENBI(const char * const netfilename, const char * const RENBI_arg) {
	print_time();
	char *p;
	double x=strtod(RENBI_arg, &p);
	struct Bipii *traini1, *traini2, *testi1, *testi2;
	create_2dataset(netfilename, &traini1, &traini2, &testi1, &testi2);

	struct iidNet *itemSim=NULL;
	get_ItemSimilarity(traini1, traini2, &itemSim);
	
	struct Metrics_Bipii *RENBI_result = RENBI_Bipii(traini1, traini2, testi1, testi2, itemSim,x);
	printf("RENBI\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", RENBI_result->R, RENBI_result->PL, RENBI_result->IL, RENBI_result->HL, RENBI_result->NL);
	free_MetricsBipii(RENBI_result);

	free_Bipii(traini1);
	free_Bipii(traini2);
	free_Bipii(testi1);
	free_Bipii(testi2);
	free_iidNet(itemSim);
	
	print_time();
	return 0;
}

int hybrid(const char * const netfilename, const char * const hybrid_arg) {
	print_time();
	char *p;
	double x=strtod(hybrid_arg, &p);
	struct Bipii *traini1, *traini2, *testi1, *testi2;
	create_2dataset(netfilename, &traini1, &traini2, &testi1, &testi2);

	struct iidNet *itemSim=NULL;
	get_ItemSimilarity(traini1, traini2, &itemSim);
	
	struct Metrics_Bipii *hybrid_result = hybrid_Bipii(traini1, traini2, testi1, testi2, itemSim, x);
	printf("hybrid\tR: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", hybrid_result->R, hybrid_result->PL, hybrid_result->IL, hybrid_result->HL, hybrid_result->NL);
	free_MetricsBipii(hybrid_result);

	free_Bipii(traini1);
	free_Bipii(traini2);
	free_Bipii(testi1);
	free_Bipii(testi2);
	free_iidNet(itemSim);
	
	print_time();
	return 0;
}
