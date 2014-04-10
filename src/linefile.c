#include "linefile.h"
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

struct LineFile *create_LineFile(const char * const filename, int vn, ...) {
	struct LineFile *lf = malloc(sizeof(struct LineFile));
	assert(lf != NULL);

	va_list vl;
	va_start(vl, filename);
	int type;
	int i;
	int *ip = lf->i1;
	int *dp = lf->d1;
	for (i = 0; i < vn; ++i) {
		switch(type = vl_arg(vl, int)) {
			case 0:
				ip = malloc(
				

		}
	}
	switch
	return NULL;
}
