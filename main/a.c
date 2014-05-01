#include "linefile.h"
#include "hash.h"
#include "base.h"
#include "bip.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc != 2) isError("argc");
	struct LineFile *lf = create_LineFile(argv[1], 1, 1, 1, 1, -1);
	long i;
	for (i = 0; i < lf->linesNum; ++i) {
		if (lf->i3[i] >= 3) {
			printf("%d\t%d\n", lf->i1[i], lf->i2[i]);	
		}
	}
	free_LineFile(lf);

	return 0;
}

