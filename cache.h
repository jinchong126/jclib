#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define CACHE_SIZE 2048
#define CACHE_MAX_NUM 10

#define log_printf printf
#define BIT(x) (1 << x)

struct cache_store {
	unsigned char *cache_head;	/* 0  */
	int cache_state; /* 01010101 */
	int cache_num;
	pthread_spinlock_t cache_lock;
};

int cache_init();
void *get_cache_store();
int put_cache_store(void *cache_data_addr);
int cache_destory();
void *cache_malloc();
int cache_free(void *cache_data_addr);
