#include "linefile.h"
#include "mtprand.h"
#include <stdio.h>

int main(void)
{
	struct LineFile *lf = create_LineFile("delicious_2c", 1, 1, -1);
	print_LineFile(lf, "1");
    return 0;
}
