#include "linefile.h"
#include "mtprand.h"
#include <stdio.h>

int main(void)
{
    int i;
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

	struct LineFile *lf = create_LineFile("test", 1, 1, 2, 9, 9);
    return 0;
}
