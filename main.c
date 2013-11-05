#include "inc/complexnet_linefile.h"
#include "stdio.h"
int main(int argc, char **argv) {
	struct i4LineFile *file = create_i4LineFile(argv[1]);
	printf("%d,%d,%d,%d\n", file->i1Max, file->i1Min, file->i2Max, file->i2Min);
	return 0;
}
