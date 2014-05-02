#include "dataset.h"

int main(int argc, char **argv)
{
	struct LineFile *lf = ER_DS(10000);
	struct iiNet *net = create_iiNet(lf);
	verify_iiNet(net);
	return 0;
}
