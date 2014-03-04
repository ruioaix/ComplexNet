#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "../linefile/i3linefile.h"

struct Snapshot{
	int maxId;
	int minId;
	char **stat;
};
void free_Snapshot(struct Snapshot *net);
struct Snapshot *create_Snapshot(const struct i3LineFile * const file);
void print_Net_SNAPSHOT(struct Snapshot *net, char *filename);

#endif
