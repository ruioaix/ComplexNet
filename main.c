#include "inc/complexnet_linefile.h"
#include "inc/complexnet_bipartite.h"
#include "inc/complexnet_random.h"

#include "stdio.h"
int main(int argc, char **argv) {
	unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
	init_by_array_MersenneTwister(init, length);

	//read the origin file
	struct i4LineFile *file = create_i4LineFile("data/hao/rating.dat");

	//create Bipartite for user
	struct Bipartite *user = create_Bipartite(file, 1);


	//abstract a basic set.
	struct i4LineFile *ab_user_1 = abstract_Bipartite(user);
	print_i4LineFile(ab_user_1, "Results/ab_user_1");

	//abstract another basic set.
	struct i4LineFile *ab_user_2 = abstract_Bipartite(user);
	print_i4LineFile(ab_user_2, "Results/ab_user_2");

	//delete all ids whose count < 20
	cutcount_Bipartite(user, 20);

	//create the i4LineFile from Bipartite
	struct i4LineFile *backtofile = backtofile_Bipartite(user);
	print_i4LineFile(backtofile, "Results/backtofile");

	//divide a i4LineFile.
	struct i4LineFile *twofile = divide_i4LineFile(backtofile, 0.2);
	if (twofile != NULL) {
		print_i4LineFile(twofile, "Results/twofile0");
		print_i4LineFile(twofile+1, "Results/twofile1");
	}

	//print two i4LineFile into one file.
	print_2_i4LineFile(twofile, ab_user_1, "Results/file1");
	//print two i4LineFile into one file.
	print_2_i4LineFile(twofile+1, ab_user_2, "Results/file2");

	//before divideBytime_Bipartite, must sortBytime_Bipartite first.
	sortBytime_Bipartite(user);
	struct i4LineFile *_2file = divideBytime_Bipartite(user, 0.2);
	if (_2file != NULL) {
		print_i4LineFile(_2file, "Results/_2file1");
		print_i4LineFile(_2file + 1, "Results/_2file2");
	}

	//end
	free_Bipartite(user);
	free_i4LineFile(file);
	free_i4LineFile(ab_user_1);
	free_i4LineFile(ab_user_2);
	free_i4LineFile(backtofile);
	free_2_i4LineFile(twofile);
	free_2_i4LineFile(_2file);
	return 0;
}
