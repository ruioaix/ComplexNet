#ifndef COMPLEXNET_THREADPOOL_H
#define COMPLEXNET_THREADPOOL_H

#include <pthread.h>

struct workOfTP{
	void* (*routine)(void*);
	void *arg;
	struct workOfTP *next;
};

struct ThreadPool{
	int destroy;
	int threadMax;
	pthread_t *threadId;
	struct workOfTP *works;
	pthread_mutex_t mutex;
	pthread_cond_t haswork;
};

int createThreadPool(int threadMax);

void destroyThreadPool(void);

int addWorktoThreadPool(void* (*routine)(void*),void *arg);

#endif
