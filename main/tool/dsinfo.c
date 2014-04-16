#include "linefile.h"
#include "hash.h"
#include "base.h"
#include "bip.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc != 2) isError("argc");
	struct LineFile *lf = create_LineFile(argv[1], 1, 1, -1);
	struct Bip *bp1 = create_Bip(lf, 1);
	struct Bip *bp2 = create_Bip(lf, 2);
	free_Bip(bp1);
	free_Bip(bp2);
	free_LineFile(lf);

	return 0;
}


//usage: ./tool-dsinfo dataset_filename
