#ifndef __RBTREE_COM_H__
#define __RBTREE_COM_H__

#include <pthread.h>
#include "rbtree.h"

struct cc{
	char addr[6];
	int i;
};

typedef int (*rbtree_compare_cb)(void *user_struct, void *mem_struct);

/*  rbtree container */
struct rbtree_container
{
	void *rbtree_data;
	struct rb_node rb_node;
};

/*  rbtree root */
struct rbtree_container_root
{
	struct rb_root rb_root;

	/*  rbtree callback function*/
	rbtree_compare_cb compare_rbtree_struct_cb;

	/*  rbtree lock*/
	pthread_spinlock_t rbtree_container_lock;
};

struct rbtree_container_root *rbtree_contarner_init(rbtree_compare_cb compare_rbtree_cb);
void rbtree_contarner_deinit(struct rbtree_container_root *rbcr);
void *container_search(struct rbtree_container_root *rbcr, void *user_data);
int container_insert(struct rbtree_container_root *rbcr, void *user_data, int rbtree_data_size);
int container_delete(struct rbtree_container_root *rbcr, void *user_data);
void container_flush(struct rbtree_container_root *rbcr);

#endif /* __RBTREE_COM_H__ */
