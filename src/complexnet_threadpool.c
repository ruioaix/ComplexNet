#include "../inc/complexnet_threadpool.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

static struct ThreadPool *TP=NULL;

static void* thread_routine(void*arg) {
	struct workOfTP *work;

	while(1){
		pthread_mutex_lock(&TP->mutex);
		while(!TP->works && !TP->destroy){
			pthread_cond_wait(&TP->haswork, &TP->mutex);
		}
		if(TP->works){
			work=TP->works;
			TP->works=TP->works->next;
			pthread_mutex_unlock(&TP->mutex);
			work->routine(work->arg);
			free(work);
			continue;
		}

		pthread_mutex_unlock(&TP->mutex);
		return NULL;
	}

	return NULL;
}

int createThreadPool(int threadMax) {
	int i;

	TP=calloc(1,sizeof(struct ThreadPool));
	assert(TP != NULL);

	TP->threadMax=threadMax;
	TP->destroy=0;
	TP->works=NULL;
	if(pthread_mutex_init(&TP->mutex,NULL)!=0){
		printf("%s:pthread_mutex_initfailed,errno:%d,error:%s\n", __FUNCTION__,errno,strerror(errno));
		exit(1);
	}
	if(pthread_cond_init(&TP->haswork,NULL)!=0){
		printf("%s:pthread_cond_initfailed,errno:%d,error:%s\n", __FUNCTION__,errno,strerror(errno));
		exit(1);
	}

	TP->threadId=calloc(threadMax, sizeof(pthread_t));
	assert(TP->threadId != NULL);
	for(i=0;i<threadMax;++i){
		if(pthread_create(TP->threadId+i, NULL, thread_routine, NULL)!=0){
			printf("%s:pthread_createfailed,errno:%d,error:%s\n",__FUNCTION__, errno,strerror(errno));
			exit(1);
		}
	}

	return 0;
}

void destroyThreadPool(void) {
	int i;

	if(TP->destroy){
		return;
	}
	TP->destroy=1;

	pthread_mutex_lock(&TP->mutex);
	pthread_cond_broadcast(&TP->haswork);
	pthread_mutex_unlock(&TP->mutex);
	for(i=0;i<TP->threadMax;++i){
		pthread_join(TP->threadId[i],NULL);
	}
	free(TP->threadId);

	struct workOfTP *member;
	while(TP->works){
		member=TP->works;
		TP->works=TP->works->next;
		free(member);
	}

	pthread_mutex_destroy(&TP->mutex);
	pthread_cond_destroy(&TP->haswork);

	free(TP);
}

int addWorktoThreadPool( void*(*routine)(void*),void*arg) {
	struct workOfTP *work,*member;
	assert(routine != NULL);

	work=malloc(sizeof(struct workOfTP));
	assert(work != NULL);

	work->routine=routine;
	work->arg=arg;
	work->next=NULL;

	pthread_mutex_lock(&TP->mutex);
	member=TP->works;
	if(!member){
		TP->works=work;
	}else{
		while(member->next){
			member=member->next;
		}
		member->next=work;
	}

	pthread_cond_signal(&TP->haswork);
	pthread_mutex_unlock(&TP->mutex);

	return 0;
}
