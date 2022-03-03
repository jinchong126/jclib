#ifndef __RCYH_RBTREE_RC_H__
#define __RCYH_RBTREE_RC_H__

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
	struct rb_node rb_node;
	void *rbtree_data;
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

int rbtree_contarner_init(rbtree_compare_cb compare_rbtree_cb);
void *container_search(void *user_data);
int container_insert(void *user_data, int rbtree_data_size);
int container_delete(void *user_data);

#endif /* __RCYH_RBTREE_RC_H__ */
