#include "linefile.h"
#include "hash.h"
#include "base.h"
#include "bip.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc != 2) isError("argc");
	struct LineFile *lf = create_LineFile(argv[1], 1, 1, -1);
	struct iiBip *bp1 = create_iiBip(lf, 1);
	struct iiBip *bp2 = create_iiBip(lf, 2);
	free_iiBip(bp1);
	free_iiBip(bp2);
	free_LineFile(lf);

	return 0;
}


//usage: ./tool-dsinfo dataset_filename
