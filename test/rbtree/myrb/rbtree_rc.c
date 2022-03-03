#include "list.h"
#include "rbtree_rc.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define log_printf printf
void display(struct cc *aa)
{
	printf("==============addr:%x:%x:%x:%x:%x:%x\n", aa->addr[0], aa->addr[1], aa->addr[2], aa->addr[3], aa->addr[4], aa->addr[5]);
	printf("==============i:%d\n", aa->i);
}

static struct rbtree_container_head g_rbtree_head;

/*******************************************************************************
 *     Function:       rbtree_lock_init
 *     Description:    红黑数锁初始化
 *     Input:
 *     Output:         
 *     Return:        
 *     Others:          <其它说明>
 *******************************************************************************/
static inline int rbtree_lock_init()
{
	pthread_spin_init(&(g_rbtree_head.rbtree_container_lock), 0);
}

static inline int rbtree_lock()
{
	pthread_spin_lock(&(g_rbtree_head.rbtree_container_lock));
}

static inline int rbtree_unlock()
{
	pthread_spin_unlock(&(g_rbtree_head.rbtree_container_lock));
}

/*
* Initializing rbtree root
*/
int rbtree_contarner_init(rbtree_compare_cb compare_rbtree_cb)
{
	if(compare_rbtree_cb == NULL) {
		log_printf("rbtree call back function is null!\n");
		return -1;
	}

	memset(&g_rbtree_head, 0, sizeof(struct rbtree_container_head));
	g_rbtree_head.rb_root = RB_ROOT;
	g_rbtree_head.compare_rbtree_struct_cb = compare_rbtree_cb;

	rbtree_lock_init();
	log_printf("rbtree contarner init completed!\n");
	return 0;
}

/*
* Search key node from rbtree
*/
void *container_search(void *user_data)
{
	int result;
	struct rb_node *node = g_rbtree_head.rb_root.rb_node;

	while (node)
	{
		struct rbtree_container *this = rb_entry(node, struct rbtree_container, rb_node);

		result = g_rbtree_head.compare_rbtree_struct_cb(user_data, this->rbtree_data);
		if (result < 0) {
			node = node->rb_left;
		} else if (result > 0) {
			node = node->rb_right;
		} else {
			return (void *)(this->rbtree_data);
		}
	}
	return NULL;
}

void *container_search2(void *user_data)
{
	int result;
	struct rb_node *node = NULL;
	struct rbtree_container *this = NULL;

	node = g_rbtree_head.rb_root.rb_node;
	while (node)
	{
		this = rb_entry(node, struct rbtree_container, rb_node);
		result = g_rbtree_head.compare_rbtree_struct_cb(user_data, this->rbtree_data);

		if (result < 0) {
			node = node->rb_left;
		} else if (result > 0) {
			node = node->rb_right;
		} else {
			return (void *)(this);
		}
	}
	return NULL;
}

/*
* Insert key node into rbtree
*/
int container_insert(void *user_data, int rbtree_data_size)
{
	int result = 0;
	struct rb_node *parent = 0;
	struct rbtree_container *rbtree_new = NULL, *this = NULL;
	struct rb_node **new = &g_rbtree_head.rb_root.rb_node;

	if(rbtree_data_size <= 0) {
		log_printf("rbtree_data_siz <= 0\n");
		return -1;
	}

    rbtree_new = (struct rbtree_container*)malloc(sizeof(struct rbtree_container) + rbtree_data_size);
	if(rbtree_new == NULL) {
		log_printf("malloc rbtree_new error\n");
		return -1;
	}

	memcpy((void *)rbtree_new + sizeof(struct rbtree_container), user_data, rbtree_data_size);
	rbtree_new->rbtree_data = (void *)rbtree_new + sizeof(struct rbtree_container);

	rbtree_lock();

	while(*new)
	{
		this = rb_entry(*new, struct rbtree_container, rb_node);

		result = g_rbtree_head.compare_rbtree_struct_cb(user_data, this->rbtree_data);

		parent = &this->rb_node;

		if (result < 0)
			new = &((*new)->rb_left);
		else if (result > 0)
			new = &((*new)->rb_right);
		else {
			rbtree_unlock();
			free(rbtree_new);
			log_printf("this date is exists\n");
			return -1;
		}
	}

	/* Add new node and rebalance tree. */
	rb_link_node(&(rbtree_new->rb_node), parent, new);
	rb_insert_color(&(rbtree_new->rb_node), &g_rbtree_head.rb_root);

	rbtree_unlock();
	return 0;
}

/*
* Delete the key node from rbtree
*     delete node from rbtree, return node pointer
*/
int container_delete(void *user_data)
{
	struct rbtree_container *rbtree_mem = NULL;

	rbtree_lock();

	rbtree_mem = container_search2(user_data);
	if (!rbtree_mem) {
		rbtree_unlock();
		log_printf("this data is not exists\n");
		return 0;
	}

	rb_erase(&rbtree_mem->rb_node, &g_rbtree_head.rb_root);

	rbtree_unlock();
	free(rbtree_mem);
	return 0;
}

void container_flush()
{
	int count = 0;
	struct rb_node *node = NULL;
	struct rbtree_container *this = NULL;

	node = g_rbtree_head.rb_root.rb_node;

	rbtree_lock();
	while(node)
	{
		this = rb_entry(node, struct rbtree_container, rb_node);
		rb_erase(&this->rb_node, &g_rbtree_head.rb_root);
		free(this);
		node = g_rbtree_head.rb_root.rb_node;
		count++;
	}
	rbtree_unlock();

	log_printf("container flush success :%d count data!\n", count);
	return ;
}
