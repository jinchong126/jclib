#ifndef __HASH_LIST__
#define __HASH_LIST__

#include <pthread.h>
#include "list.h"

#ifndef BASE_HASH_TABLE_MAX
#define BASE_HASH_TABLE_MAX  (1<<14)
#endif

typedef enum {
	HASH_NEED_LOCK_NONE, /* None Lock */
	HASH_NEED_LOCK_SPIN, /* Spin Lock */
	HASH_NEED_LOCK_MUTEX, /* Mutex Lock */
} hash_table_need_lock;

/* hash table callback function*/
typedef unsigned int (*base_hash_val_cb)(void *user_struct);
typedef int (*base_hash_compare_cb)(void *user_struct, void *mem_struct);
typedef int (*base_hash_echo_node_cb)(void *user_struct, void *arg);

/* hash table head */
struct base_hash_table_head
{
	struct hlist_head *hash_table_head;
	pthread_spinlock_t hash_lock_spin;
	pthread_mutex_t hash_lock_mutex;
};

/* hash table list */
struct base_hash_table_list
{
	void *struct_head;
	struct hlist_node list;
};

struct hash_table_opt
{
	/* Name of the hash table */
	char name[16];

	/* hash table head number */
	unsigned int hash_size;

	/* need lock */
	int need_lock;

	/* calculate hash values callback */
	base_hash_val_cb hash_val_cb;

	/* compare key values callback */
	base_hash_compare_cb cmp_cb;

	struct base_hash_table_head hdr;
};

int hash_table_init(struct hash_table_opt *opt);

void *hash_table_add(struct hash_table_opt *opt, void *user_struct, int struct_size);

int hash_table_del(struct hash_table_opt *opt, void *user_struct);

int hash_table_del_by_addr(struct hash_table_opt *opt, void *user_struct, int size);

void hash_table_flush(struct hash_table_opt *opt);

void *hash_table_lookup_user(struct hash_table_opt *opt, void *user_struct);

void *hash_table_next_by_user(struct hash_table_opt *opt, void *user_struct);

int hash_table_each_node(struct hash_table_opt *opt, base_hash_echo_node_cb cb, void *arg);

int hash_table_each_node_by_range(struct hash_table_opt *opt, int begin, int end, base_hash_echo_node_cb cb, void *arg);

void *hash_table_reusable_add(struct hash_table_opt *opt, void *user_struct, int struct_size);

int hash_table_reusable_del(struct hash_table_opt *opt, void *user_struct);
#endif 
