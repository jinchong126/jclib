#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define CACHE_SIZE 2048
#define CACHE_MAX_NUM 10

#define log_printf printf
#define BIT(x) (1 << x)

struct cache_store {
	unsigned char *data_head;	/* 0  */
	int map; /* 01010101 */
	int num;
	pthread_spinlock_t cache_lock;
};

struct cache_store *cache_init();
void *get_cache_store(struct cache_store *cs);
int put_cache_store(struct cache_store *cs, void *cache_data_addr);
int cache_destory(struct cache_store *cs);
void *cache_malloc(struct cache_store *cs);
int cache_free(struct cache_store *cs, void *cache_data_addr);
