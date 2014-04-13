#include "linefile.h"
#include "mtprand.h"
#include <stdio.h>

int main(void)
{
	int i;
	struct LineFile *lf = create_LineFile("delicious_2c", 1, 1, -1);
	printf("1000 outputs of genrand_int32()\n");
	for (i=0; i<1000; i++) {
		printf("%10lu ", get_i32_MTPR());
		if (i%5==4) printf("\n");
	}
	printf("\n1000 outputs of genrand_real2()\n");
	for (i=0; i<1000; i++) {
		printf("%10.8f ", get_d0_MTPR());
		if (i%5==4) printf("\n");
	}
	print_LineFile(lf, "1");
	free_LineFile(lf);

	return 0;
}
