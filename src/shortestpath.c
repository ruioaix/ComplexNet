#include "../inc/shortestpath.h"

long *getOne2AllTSP(struct Net *net, int vtId)
{
	long *sp=malloc(net->vtNum*sizeof(long));
	memError(sp, "getOne2AllSP sp");
	/*
	int i;
	for (i=0; i<net->vtNum; i++) {
		sp[i]=LONG_MAX;
	}
	
	uint32_t *bitsign=createBitSign(net->vtNum);
	setBitSign(bitsign, vtId);
	sp[vtId]=0;

	int k=0;

	int remain=net->vtNum-1;
	int current=vtId;
	int weight=0;
	while(remain>0) {
		//printf("current:%7d\tweight:%d\tremain:%d\n", current, weight, remain);
		int i;
		int shortest=INT_MAX, record, neigh;
		for (i=0; i<net->count[current]; i++) {
			neigh=net->rela[current][i];
			if (!getBitSign(bitsign, neigh)) {
				int neighWei=net->time[current][i];
				if (weight<=neighWei) {
					sp[neigh]=neighWei<sp[neigh]?neighWei:sp[neigh];
					if (sp[neigh]<=shortest) {
						shortest=sp[neigh];
						record=neigh;
					}
					//printf("record:%d\tshortest:%d\tsp[neigh]:%ld\n", record, shortest, sp[neigh]);
				}
			}
		}
		if (shortest==INT_MAX) {
			++k;
			int sign=0;
			for (i=0; i<net->vtNum; i++) {
				if (sp[i]!=LONG_MAX && !getBitSign(bitsign, i)) {
					setBitSign(bitsign, i);
					--remain;
					current=i;
					weight=sp[i];
					sign=1;
					break;
				}
			}
			if (sign==0) {
				printf("over\n");
				break;
			}
		} else {
			setBitSign(bitsign, record);
			--remain;
			current=record;
			weight=shortest;
		}
	}
	printf("k: %d\n",k);
	*/
	return sp;
}

//long *getOne2AllSP(struct Net *net, int vtId)
//{
//	long *sp=malloc(net->vtNum*sizeof(long));
//	memError(sp, "getOne2AllSP sp");
//	int i;
//	for (i=0; i<net->vtNum; i++) {
//		sp[i]=LONG_MAX;
//	}
//	
//	uint32_t *bitsign=createBitSign(net->vtNum);
//	int remain=net->vtNum-1;
//	int current=vtId;
//	sp[current]=0;
//
//	int weight=0;
//	while(remain>0) {
//		int i;
//		int shortest=INT_MAX, record;
//		for (i=0; i<net->count[current]; i++) {
//			int neigh=net->rela[current][i];
//			int neighWei=net->time[current][i]+weight;
//			sp[neigh]=neighWei>sp[neigh]?sp[neigh]:neighWei;
//
//			if (shortest>=neighWei && !getBitSign(bitsign, neigh)) {
//				shortest=neighWei;
//				record=neigh;
//			}
//		}
//		
//		
//	}
//}
//
