#include "test.h"
#include "linefile.h"
#include <assert.h>

int main(int argc, char **argv)
{
	test_linefile();
	int i;
	for (i = 6; i < 11; ++i) {
		double lambda = i*0.1;
		int L = 5000;
		test_sp1(lambda, L);
	}
	return 0;
}
