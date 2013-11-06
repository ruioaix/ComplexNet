#include "inc/complexnet_linefile.h"
#include "inc/complexnet_bipartite.h"
#include "inc/complexnet_random.h"

#include "stdio.h"
int main(int argc, char **argv) {
	//end
	struct i4LineFile *file = create_i4LineFile(argv[1]);
	printf("%d,%d,%d,%d\n", file->i1Max, file->i1Min, file->i2Max, file->i2Min);
	struct Bipartite *user = create_Bipartite(file, 1);
	struct Bipartite *item = create_Bipartite(file, 0);

	unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
	init_by_array_MersenneTwister(init, length);


	//end
	free_Bipartite(user);
	free_Bipartite(item);
	return 0;
}
