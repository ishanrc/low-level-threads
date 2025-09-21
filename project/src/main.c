#include "thread_pool.h"
#include <stdint.h>
#include <stdio.h>



void test_task(void* arg) {
	printf("ttt\n");
	int64_t v = (int64_t)arg;
	printf("task: %ld\n", v);

}

int main() {
	threadpool_t thread_pool;
	threadpool_init(&thread_pool);
	for (int64_t i = 0; i < 20; ++i) {
		threadpool_add_task(&thread_pool, &test_task, (void*)i);
	}
	threadpool_destroy(&thread_pool);
	return 0;
}
