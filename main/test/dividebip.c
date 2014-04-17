/**
 * test divide_Bip function in bip.c
 *
 * give a ds, use divide_Bip to divide it into two parts.
 * print 
 */
#include "base.h"
#include "linefile.h"
#include "bip.h"

int main(int argc, char **argv)
{
	char *filename;
	if (argc == 1) {
		filename = "movielens_3c";
	}
	else if(argc == 2) {
		filename = argv[1];
	}
	else {
		isError("argv");
	}
	struct LineFile *lf = create_LineFile(filename,  1, 1, 1, -1);
	print_label();
	struct Bip *b1 = create_Bip(lf, 1);
	struct Bip *b2 = create_Bip(lf, 2);
	struct LineFile *big, *small;
	divide_Bip(b1, b2, 0.1, &small, &big);
	struct LineFile *add = add_LineFile(small, big);
	struct Bip *a1 = create_Bip(add, 1);
	sort_desc_Bip(a1);
	sort_desc_Bip(b1);
	//print_Bip(a1, "a1");
	//print_Bip(b1, "b1");
	if (b1->maxId != a1->maxId) isError("divide_Bip has bugs: maxId");
	int i;
	long j;
	for (i = 0; i < a1->maxId + 1; ++i) {
		if (b1->count[i] != a1->count[i]) {
			isError("divide_Bip has bugs: count");
		}	
		else {
			for (j = 0; j < a1->count[i]; ++j) {
				if (a1->edges[i][j] != b1->edges[i][j]) {
					printf("%d\t%d\t%d\n", i, a1->edges[i][j], b1->edges[i][j]);
					isError("divide_Bip has bugs: edges");
				}	
				if (a1->score != NULL && b1->score != NULL) {
					if (a1->score[i][j] != b1->score[i][j]) {
						isError("divide_Bip has bugs: score value");
					}
				}
				else if (a1->score == NULL && b1->score == NULL) {
				}
				else {
					isError("divide_Bip has bugs: score");
				}
			}
		}
	}
	printf("test divide_Bip =>> perfect.\n");
	free_LineFile(lf);
	free_LineFile(small);
	free_LineFile(big);
	free_LineFile(add);
	free_Bip(a1);
	free_Bip(b1);
	free_Bip(b2);
	return 0;
}
