#include "cache.h"

struct cache_store g_cache_store;

static inline int cache_lock_init()
{
	pthread_spin_init(&(g_cache_store.cache_lock), 0);
	return 0;
}

static inline int cache_lock()
{
	pthread_spin_lock(&(g_cache_store.cache_lock));
	return 0;
}

static inline int cache_unlock()
{
	pthread_spin_unlock(&(g_cache_store.cache_lock));
	return 0;
}

int cache_init()
{
	g_cache_store.cache_head = NULL;

	g_cache_store.cache_head = malloc(CACHE_SIZE * CACHE_MAX_NUM);

	if(g_cache_store.cache_head == NULL) {
		log_printf("malloc cache store error!\n");
		return -1;
	}
	g_cache_store.cache_state = 0;
	g_cache_store.cache_num = 0;
	cache_lock_init();
	log_printf("cache store init ok!\n");
	return 0;
}

void *get_cache_store()
{
	int i, exists = 0;
	if(g_cache_store.cache_num >= CACHE_MAX_NUM) {
		log_printf("cache is full\n");
		return NULL;
	}
	cache_lock();
	for(i = 0; i <= CACHE_MAX_NUM; i++) {
		if(!(BIT(i) & g_cache_store.cache_state)) {
			exists = 1;
			break;
		}
	}

	if(exists) {
		g_cache_store.cache_state |= BIT(i);
		g_cache_store.cache_num++;
		cache_unlock();
		return g_cache_store.cache_head + i * CACHE_SIZE;
	}
	cache_unlock();
	return NULL;
}

int check_addr_is_cache_addr(void *cache_data_addr)
{
	int i = 0, j = 0;

	j = (cache_data_addr - (void *)g_cache_store.cache_head) % CACHE_SIZE;
	if(j != 0) {
		return -1;
	}

	i = (cache_data_addr - (void *)g_cache_store.cache_head) / CACHE_SIZE;
	if(i < 0 || i >= CACHE_MAX_NUM) {
		return -1;
	}
	return i;
}

int put_cache_store(void *cache_data_addr)
{
	int i = 0;

	i = check_addr_is_cache_addr(cache_data_addr);
	if(i < 0) {
		log_printf("cache data addr error!\n");
		return -1;
	}

	cache_lock();
	g_cache_store.cache_state &= ~BIT(i);
	g_cache_store.cache_num--;
	cache_unlock();

	return 0;
}

int cache_destory()
{
	free(g_cache_store.cache_head);
	g_cache_store.cache_head = NULL;
	g_cache_store.cache_state = 0;
	g_cache_store.cache_num = 0;
	log_printf("cache destory is ok!\n");
	return 0;
}

void *cache_malloc()
{
	void *mem_space = NULL;
	mem_space = get_cache_store();
	if(mem_space == NULL) {
		mem_space = malloc(CACHE_SIZE);
		if(mem_space == NULL) {
			log_printf("cache malloc error!\n");
			return NULL;
		}
	}
	return mem_space;
}

int cache_free(void *cache_data_addr)
{
	int i;
	i = check_addr_is_cache_addr(cache_data_addr);
	if(i < 0)
		free(cache_data_addr);
	return 0;
}

int main()
{
	void *recv_data = NULL;
	void *recv_data2 = NULL;
	void *recv_data3 = NULL;
	cache_init();
	recv_data = get_cache_store();
	recv_data = get_cache_store();
	recv_data = get_cache_store();
	recv_data = get_cache_store();
	recv_data3 = get_cache_store();
	recv_data = get_cache_store();
	recv_data = get_cache_store();
	recv_data = get_cache_store();
	recv_data2 = get_cache_store();
	recv_data = get_cache_store();
	recv_data = get_cache_store();
/*
	recv_data = get_cache_store();
	recv_data = get_cache_store();
	recv_data = get_cache_store();
	recv_data = get_cache_store();
	recv_data = get_cache_store();
*/
log_printf("get_cache:%p\n", recv_data);
log_printf("cache_num:%d\n", g_cache_store.cache_num);
	put_cache_store(recv_data2);
	put_cache_store(recv_data3);
log_printf("cache_num:%d\n", g_cache_store.cache_num);
	recv_data = get_cache_store();
	if(recv_data == NULL) {
		log_printf("cache store get error!\n");
	}else{
		log_printf("state:%02x\n", g_cache_store.cache_state);
	}
	log_printf("state:%d \n", g_cache_store.cache_state);
log_printf("cache_num:%d\n", g_cache_store.cache_num);
	return 0;
}
