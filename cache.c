#include "cache.h"

static inline int cache_lock_init(struct cache_store *cs)
{
	pthread_spin_init(&(cs->cache_lock), 0);
	return 0;
}

static inline int cache_lock(struct cache_store *cs)
{
	pthread_spin_lock(&(cs->cache_lock));
	return 0;
}

static inline int cache_unlock(struct cache_store *cs)
{
	pthread_spin_unlock(&(cs->cache_lock));
	return 0;
}

struct cache_store *cache_init()
{
    struct cache_store *cs;
	cs = (struct cache_store *)malloc(sizeof(struct cache_store));
	if (cs == NULL) {
		log_printf("Malloc cs failure, Init error\n");
		return NULL;
    }

	cs->data_head = malloc(CACHE_SIZE * CACHE_MAX_NUM);

	if (cs->data_head == NULL) {
		log_printf("malloc cache store error!\n");
        free(cs);
		return NULL;
	}
	cs->map = 0;
	cs->num = 0;
	cache_lock_init(cs);
	log_printf("cache store init ok!\n");
	return cs;
}

void cache_deinit(struct cache_store *cs)
{
    if (cs == NULL) {
        return;
    }

    if (cs->data_head != NULL) {
        free(cs->data_head);
    }

    free(cs);
    return;
}

void *get_cache_store(struct cache_store *cs)
{
	int i, exists = 0;
	if(cs->num >= CACHE_MAX_NUM) {
		log_printf("cache is full\n");
		return NULL;
	}
	cache_lock(cs);
	for(i = 0; i <= CACHE_MAX_NUM; i++) {
		if(!(BIT(i) & cs->map)) {
			exists = 1;
			break;
		}
	}

	if(exists) {
		cs->map |= BIT(i);
		cs->num++;
		cache_unlock(cs);
		return cs->data_head + i * CACHE_SIZE;
	}
	cache_unlock(cs);
	return NULL;
}

int check_addr_is_cache_addr(struct cache_store *cs, void *cache_data_addr)
{
	int i = 0, j = 0;

	j = (cache_data_addr - (void *)cs->data_head) % CACHE_SIZE;
	if(j != 0) {
		return -1;
	}

	i = (cache_data_addr - (void *)cs->data_head) / CACHE_SIZE;
	if(i < 0 || i >= CACHE_MAX_NUM) {
		return -1;
	}
	return i;
}

int put_cache_store(struct cache_store *cs, void *cache_data_addr)
{
	int i = 0;

	i = check_addr_is_cache_addr(cs, cache_data_addr);
	if(i < 0) {
		log_printf("cache data addr error!\n");
		return -1;
	}

	cache_lock(cs);
	cs->map &= ~BIT(i);
	cs->num--;
	cache_unlock(cs);

	return 0;
}

int cache_destory(struct cache_store *cs)
{
	free(cs->data_head);
	cs->data_head = NULL;
	cs->map = 0;
	cs->num = 0;
	log_printf("cache destory is ok!\n");
	return 0;
}

void *cache_malloc(struct cache_store *cs)
{
	void *mem_space = NULL;
	mem_space = get_cache_store(cs);
	if(mem_space == NULL) {
		mem_space = malloc(CACHE_SIZE);
		if(mem_space == NULL) {
			log_printf("cache malloc error!\n");
			return NULL;
		}
	}
	return mem_space;
}

int cache_free(struct cache_store *cs, void *cache_data_addr)
{
	int i;
	i = check_addr_is_cache_addr(cs, cache_data_addr);
	if(i < 0)
		free(cache_data_addr);
	return 0;
}

int main()
{
	void *recv_data = NULL;
	void *recv_data2 = NULL;
	void *recv_data3 = NULL;
    struct cache_store cs;

	cache_init(&cs);
	recv_data = get_cache_store(&cs);
	recv_data = get_cache_store(&cs);
	recv_data = get_cache_store(&cs);
	recv_data = get_cache_store(&cs);
	recv_data3 = get_cache_store(&cs);
	recv_data = get_cache_store(&cs);
	recv_data = get_cache_store(&cs);
	recv_data = get_cache_store(&cs);
	recv_data2 = get_cache_store(&cs);
	recv_data = get_cache_store(&cs);
	recv_data = get_cache_store(&cs);
log_printf("get_cache:%p\n", recv_data);
log_printf("cache_num:%d\n", cs.num);
	put_cache_store(&cs, recv_data2);
	put_cache_store(&cs, recv_data3);
log_printf("cache_num:%d\n", cs.num);
	recv_data = get_cache_store(&cs);
	if(recv_data == NULL) {
		log_printf("cache store get error!\n");
	}else{
		log_printf("state:%02x\n", cs.map);
	}
	log_printf("state:%d \n", cs.map);
log_printf("cache_num:%d\n", cs.num);
	return 0;
}
