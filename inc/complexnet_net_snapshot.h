#ifndef COMPLEXNET_NET_SNAPSHOT_H
#define COMPLEXNET_NET_SNAPSHOT_H

#include "../inc/complexnet_linefile.h"

struct Net_SNAPSHOT{
	int maxId;
	int minId;
	char **stat;
};
void free_Net_SNAPSHOT(void);
struct Net_SNAPSHOT* get_Net_SNAPSHOT(void);
void create_Net_SNAPSHOT(const struct i3LineFile * const file);
//int find_Net_SNAPSHOT_status(int infect_source, int eye);


#endif
