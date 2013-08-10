#include "../inc/complexnet_dnet.h"

struct DirectNet *buildDNet(struct NetFile *file) {
	idtype maxId=file->maxId;
	idtype minId=file->minId;
	linesnumtype linesNum=file->linesNum;
	struct LineInfo *lines=file->lines;

	linesnumtype *count=calloc(maxId+1, sizeof(linesnumtype));
	char *status=calloc(maxId+1, sizeof(char));
	assert(count!=NULL);
	assert(status!=NULL);

	linesnumtype i;
	for(i=0; i<linesNum; i++) {
		++count[lines[i].vt1Id];
	}
	
	idtype **to=calloc(maxId+1, sizeof(void *));
	assert(to!=NULL);
	linesnumtype countMax=0;
	for(i=0; i<maxId+1; i++) {
		if (countMax<count[i]) {
			countMax=count[i];
		}
		if (count[i]!=0) {
			to[i]=malloc(count[i]*sizeof(idtype));
			assert(to[i]!=NULL);
		}
	}

	linesnumtype *temp_count=calloc(maxId+1, sizeof(linesnumtype));
	assert(temp_count!=NULL);
	for(i=0; i<linesNum; i++) {
		idtype id_from=lines[i].vt1Id;
		idtype id_to=lines[i].vt2Id;
		to[id_from][temp_count[id_from]]=id_to;
		++temp_count[id_from];
	}
	free(temp_count);

	struct DirectNet *dnet=malloc(sizeof(struct DirectNet));
	dnet->maxId=maxId;
	dnet->minId=minId;
	dnet->edgesNum=linesNum;
	dnet->countMax=countMax;
	dnet->count=count;
	dnet->status=status;
	dnet->to=to;
	
	return dnet;
}
