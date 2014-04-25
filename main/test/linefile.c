#include "linefile.h"
#include "iinet.h"
#include "dataset.h"
int main(int argc, char **argv)
{
	//parts45_DS("45");	
	struct LineFile *lf = create_LineFile("45", 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5, -1);
	//struct LineFile *lf = line1d_DS(5510, CYCLE, NON_DIRECT);
	//struct iiNet *net = create_iiNet(lf);
	//print_iiNet(net, "yy");
	print_LineFile(lf, "xx");
	free_LineFile(lf);
	//free_iiNet(net);
	return 0;
}
