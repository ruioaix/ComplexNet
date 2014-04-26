#include "test.h"
#include <stdio.h>
#include <assert.h>
#include "linefile.h"

static test_init_LineFile(struct LineFile *lf) {
	assert(NULL != lf);
	assert(0 == lf->linesNum);
	assert(1 == lf->memNum);
}

static void test_create_LineFile(void) {
	struct LineFile *lf = create_LineFile(NULL);
	test_init_LineFile(lf);

}


static void test_print_LineFile(void) {
}

static void test_free_LineFile(void) {
}

static void test_add_LineFile(void) {
}

void test_linefile(void) {
	test_create_LineFile();
	//parts45_DS("45", 1000000, 7, 6, 5, 4, 3);	
	//struct LineFile *lf = create_LineFile("45", 1, 1, 1, 1, 1, 1, 1,  2, 2, 2, 2, 2, 2, 3,3,3,3,3,4,4,4,4,5,5,5, -1);
}

