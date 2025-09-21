#include "threadpool.h"
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>


void* thread_loop(void* arg) {
	threadpool_t* pool = (threadpool_t*)arg;
	while (true) {
		pthread_mutex_lock(&pool->lock);
		while (pool->stop == 0 && pool->queued == 0) {
			pthread_cond_wait(&pool->notify, &pool->lock);
		}
		if (pool->queued > 0)
		{
			assert(pool->queued > 0);
			// consume the task
			printf("dequeue task at %d\n", pool->queue_front);
			task_t task = pool->task_queue[pool->queue_front];
			pool->queue_front++;
			if (pool->queue_front == QUEUE_SIZE) {
				pool->queue_front = 0;
			}
			pool->queued--;
			pthread_mutex_unlock(&pool->lock);
			task.fn(task.arg);
		} else {
			assert(pool->stop > 0);
			pthread_mutex_unlock(&pool->lock);
			printf("worker thread stopping\n");
			return NULL;
		}
	}
}

void threadpool_init(threadpool_t* pool) {
	// initialize fields
	pthread_mutex_init(&pool->lock, NULL);
	pthread_cond_init(&pool->notify, NULL);
	pool->queued = 0;
	pool->queue_front = 0;
	pool->queue_back = 0;
	pool->stop = 0;
	for (int i = 0; i < QUEUE_SIZE; i++) {
		pool->task_queue[i].fn = NULL;
		pool->task_queue[i].arg = NULL;
	}
	pthread_mutex_lock(&pool->lock);
	for (int i = 0; i < THREADS; ++i) {
		pthread_create(&pool->threads[i], NULL, &thread_loop, (void*)pool);
	}
	pthread_mutex_unlock(&pool->lock);
}
void threadpool_destroy(threadpool_t* pool) {
	pthread_mutex_lock(&pool->lock);
	pool->stop = 1;
	pthread_cond_broadcast(&pool->notify);
	pthread_mutex_unlock(&pool->lock);
	for (int i = 0; i < THREADS; ++i) {
		int join_ret = pthread_join(pool->threads[i], NULL);
		assert(join_ret == 0);
	}
	pthread_mutex_destroy(&pool->lock);
	pthread_cond_destroy(&pool->notify);
	printf("thread pool destroyed.\n");
}

void threadpool_add_task(threadpool_t* pool, void (*function)(void*), void* arg) {
	pthread_mutex_lock(&pool->lock);
	assert(pool->queued < QUEUE_SIZE);
	pool->task_queue[pool->queue_back].fn = function;
	pool->task_queue[pool->queue_back].arg = arg;
	printf("added task at index %d\n", pool->queue_back);
	pool->queue_back++;
	if (pool->queue_back == QUEUE_SIZE) {
		pool->queue_back = 0;
	}
	pool->queued++;
	pthread_cond_signal(&pool->notify);
	pthread_mutex_unlock(&pool->lock);
}

void example_task(void* arg) {
	printf("example task\n");
}

