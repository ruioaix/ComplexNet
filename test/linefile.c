#include "test.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "linefile.h"

static void test_init_LineFile(struct LineFile *lf) {
	assert(NULL != lf);
	assert(0 == lf->linesNum);
	assert(0 == lf->memNum);
	assert(NULL != lf->filename);
	assert(0 == strcmp(lf->filename, "NewEmptyLF"));

	assert(9 == lf->iNum);
	assert(9 == lf->dNum);
	assert(9 == lf->cNum);
	assert(9 == lf->lNum);
	assert(9 == lf->ccNum);

	assert(lf->ilist[0] == &(lf->i1));
	assert(lf->ilist[1] == &(lf->i2));
	assert(lf->ilist[2] == &(lf->i3));
	assert(lf->ilist[3] == &(lf->i4));
	assert(lf->ilist[4] == &(lf->i5));
	assert(lf->ilist[5] == &(lf->i6));
	assert(lf->ilist[6] == &(lf->i7));
	assert(lf->ilist[7] == &(lf->i8));
	assert(lf->ilist[8] == &(lf->i9));
	assert(lf->dlist[0] == &(lf->d1));
	assert(lf->dlist[1] == &(lf->d2));
	assert(lf->dlist[2] == &(lf->d3));
	assert(lf->dlist[3] == &(lf->d4));
	assert(lf->dlist[4] == &(lf->d5));
	assert(lf->dlist[5] == &(lf->d6));
	assert(lf->dlist[6] == &(lf->d7));
	assert(lf->dlist[7] == &(lf->d8));
	assert(lf->dlist[8] == &(lf->d9));
	assert(lf->llist[0] == &(lf->l1));
	assert(lf->llist[1] == &(lf->l2));
	assert(lf->llist[2] == &(lf->l3));
	assert(lf->llist[3] == &(lf->l4));
	assert(lf->llist[4] == &(lf->l5));
	assert(lf->llist[5] == &(lf->l6));
	assert(lf->llist[6] == &(lf->l7));
	assert(lf->llist[7] == &(lf->l8));
	assert(lf->llist[8] == &(lf->l9));
	assert(lf->clist[0] == &(lf->c1));
	assert(lf->clist[1] == &(lf->c2));
	assert(lf->clist[2] == &(lf->c3));
	assert(lf->clist[3] == &(lf->c4));
	assert(lf->clist[4] == &(lf->c5));
	assert(lf->clist[5] == &(lf->c6));
	assert(lf->clist[6] == &(lf->c7));
	assert(lf->clist[7] == &(lf->c8));
	assert(lf->clist[8] == &(lf->c9));
	assert(lf->cclist[0] == &(lf->cc1));
	assert(lf->cclist[1] == &(lf->cc2));
	assert(lf->cclist[2] == &(lf->cc3));
	assert(lf->cclist[3] == &(lf->cc4));
	assert(lf->cclist[4] == &(lf->cc5));
	assert(lf->cclist[5] == &(lf->cc6));
	assert(lf->cclist[6] == &(lf->cc7));
	assert(lf->cclist[7] == &(lf->cc8));
	assert(lf->cclist[8] == &(lf->cc9));

	assert(NULL == (lf->i1));
	assert(NULL == (lf->i2));
	assert(NULL == (lf->i3));
	assert(NULL == (lf->i4));
	assert(NULL == (lf->i5));
	assert(NULL == (lf->i6));
	assert(NULL == (lf->i7));
	assert(NULL == (lf->i8));
	assert(NULL == (lf->i9));
	assert(NULL == (lf->d1));
	assert(NULL == (lf->d2));
	assert(NULL == (lf->d3));
	assert(NULL == (lf->d4));
	assert(NULL == (lf->d5));
	assert(NULL == (lf->d6));
	assert(NULL == (lf->d7));
	assert(NULL == (lf->d8));
	assert(NULL == (lf->d9));
	assert(NULL == (lf->l1));
	assert(NULL == (lf->l2));
	assert(NULL == (lf->l3));
	assert(NULL == (lf->l4));
	assert(NULL == (lf->l5));
	assert(NULL == (lf->l6));
	assert(NULL == (lf->l7));
	assert(NULL == (lf->l8));
	assert(NULL == (lf->l9));
	assert(NULL == (lf->c1));
	assert(NULL == (lf->c2));
	assert(NULL == (lf->c3));
	assert(NULL == (lf->c4));
	assert(NULL == (lf->c5));
	assert(NULL == (lf->c6));
	assert(NULL == (lf->c7));
	assert(NULL == (lf->c8));
	assert(NULL == (lf->c9));
	assert(NULL == (lf->cc1));
	assert(NULL == (lf->cc2));
	assert(NULL == (lf->cc3));
	assert(NULL == (lf->cc4));
	assert(NULL == (lf->cc5));
	assert(NULL == (lf->cc6));
	assert(NULL == (lf->cc7));
	assert(NULL == (lf->cc8));
	assert(NULL == (lf->cc9));

}

static void test_create_LineFile(void) {
	/************test wrong input****************************************************************************/
	struct LineFile *lf = create_LineFile(NULL);
	test_init_LineFile(lf);
	lf = create_LineFile("anyfile", -1);
	test_init_LineFile(lf);
	lf = create_LineFile("anyfile", 1, 2, 1);
	test_init_LineFile(lf);
	lf = create_LineFile("anyfile", 11.2);
	test_init_LineFile(lf);
	lf = create_LineFile("anyfile", 11.2, 2);
	test_init_LineFile(lf);
	lf = create_LineFile("anyfile", 11.2, -1);
	test_init_LineFile(lf);
	lf = create_LineFile("anyfile", 'A');
	test_init_LineFile(lf);
	lf = create_LineFile("anyfile", 2);
	test_init_LineFile(lf);
	lf = create_LineFile("anyfile", -2.3, 8, -1);
	test_init_LineFile(lf);
	/********************************************************************************************************/

	/*********test output with corret right intput***********************************************************/
	//lf = create_LineFile("./test/data/testdata", 1, 1, -1);
	/********************************************************************************************************/

}

void test_linefile(void) {
	test_create_LineFile();
	//parts45_DS("45", 1000000, 7, 6, 5, 4, 3);	
	//struct LineFile *lf = create_LineFile("45", 1, 1, 1, 1, 1, 1, 1,  2, 2, 2, 2, 2, 2, 3,3,3,3,3,4,4,4,4,5,5,5, -1);
}
