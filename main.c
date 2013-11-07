#include "inc/complexnet_linefile.h"
#include "inc/complexnet_bipartite.h"
#include "inc/complexnet_random.h"

#include "stdio.h"
int main(int argc, char **argv) {
	unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
	init_by_array_MersenneTwister(init, length);

	struct i4LineFile *file = create_i4LineFile("data/hao/rating.dat");

	struct Bipartite *user = create_Bipartite(file, 1);
	struct i4LineFile *ab_user_1 = abstract_Bipartite(user);
	print_i4LineFile(ab_user_1, "Results/ab_user_1");
	struct i4LineFile *ab_user_2 = abstract_Bipartite(user);
	print_i4LineFile(ab_user_2, "Results/ab_user_2");

	struct i4LineFile *backtofile = backtofile_Bipartite(user);
	print_i4LineFile(backtofile, "Results/backtofile");

	struct i4LineFile *twofile = divide_i4LineFile(backtofile, 0.2);
	print_i4LineFile(twofile, "Results/twofile0");
	print_i4LineFile(twofile+1, "Results/twofile1");

	print_2_i4LineFile(twofile, ab_user_1, "Results/file1");
	print_2_i4LineFile(twofile+1, ab_user_2, "Results/file2");


	struct Bipartite *item = create_Bipartite(file, 0);
	struct i4LineFile *ab_item = abstract_Bipartite(item);


	//end
	free_Bipartite(user);
	free_Bipartite(item);
	return 0;
}
