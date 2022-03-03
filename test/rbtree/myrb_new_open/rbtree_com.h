#ifndef __RBTREE_COM_H__
#define __RBTREE_COM_H__

#include <pthread.h>
#include "rbtree.h"

typedef int (*rbtree_compare_cb)(void *user_struct, void *mem_struct);

/*  rbtree container */
struct rbtree_container
{
	void *rbtree_data;
	struct rb_node rb_node;
};

/*  rbtree root */
struct rbtree_container_head
{
	struct rb_root rb_root;

	/*  rbtree callback function*/
	rbtree_compare_cb compare_rbtree_struct_cb;

	/*  rbtree lock*/
	pthread_spinlock_t rbtree_container_lock;
};

int rbtree_contarner_init(struct rbtree_container_head *sta_rbtree, rbtree_compare_cb compare_rbtree_cb);
void *container_search(struct rbtree_container_head *sta_rbtree, void *user_data);
int container_insert(struct rbtree_container_head *sta_rbtree, void *user_data, int rbtree_data_size);
int container_delete(struct rbtree_container_head *sta_rbtree, void *user_data);

#endif /* __RBTREE_COM_H__ */
