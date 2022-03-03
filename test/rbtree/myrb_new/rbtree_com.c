#include "list.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "rbtree_com.h"
#define MSG_DEBUG 1
int wpa_debug_level = 1;

void wpa_printf(int level, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if (level >= wpa_debug_level) {
		vprintf(fmt, ap);
		printf("\n");
	}
	va_end(ap);
}
/*******************************************************************************
 *     Function:       rbtree_lock_init
 *     Description:    
 *     Input:
 *     Output:         
 *     Return:        
 *     Others:         
 *******************************************************************************/
static inline int rbtree_lock_init(struct rbtree_container_head *sta_rbtree)
{
	//pthread_mutexattr_t attr;
	//pthread_mutexattr_init(&attr);
	//pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);

	//pthread_mutex_init(&(sta_rbtree->rbtree_container_lock), &attr);
	//pthread_mutex_init(&(sta_rbtree->rbtree_container_lock), 0);
	pthread_spin_init(&(sta_rbtree->rbtree_container_lock), 0);
	return 0;
}

static inline int rbtree_lock(struct rbtree_container_head *sta_rbtree)
{
	pthread_spin_lock(&(sta_rbtree->rbtree_container_lock));
	return 0;
}

static inline int rbtree_unlock(struct rbtree_container_head *sta_rbtree)
{
	pthread_spin_unlock(&(sta_rbtree->rbtree_container_lock));
	return 0;
}

/*
* Initializing rbtree root
*/
int rbtree_contarner_init(struct rbtree_container_head *sta_rbtree, rbtree_compare_cb compare_rbtree_cb)
{
	if(compare_rbtree_cb == NULL) {
		wpa_printf(MSG_DEBUG, "rbtree call back function is null!");
		return -1;
	}

	memset(sta_rbtree, 0, sizeof(struct rbtree_container_head));
	sta_rbtree->rb_root = RB_ROOT;
	sta_rbtree->compare_rbtree_struct_cb = compare_rbtree_cb;

	rbtree_lock_init(sta_rbtree);
	wpa_printf(MSG_DEBUG, "rbtree contarner init completed!");
	return 0;
}

/*
* Search key node from rbtree
*/
void *container_search(struct rbtree_container_head *sta_rbtree, void *user_data)
{
	int result;
	struct rb_node *node = sta_rbtree->rb_root.rb_node;
	while (node)
	{
		struct rbtree_container *this = rb_entry(node, struct rbtree_container, rb_node);
		result = sta_rbtree->compare_rbtree_struct_cb(user_data, this->rbtree_data);
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

void *container_search2(struct rbtree_container_head *sta_rbtree, void *user_data)
{
	int result;
	struct rb_node *node = NULL;
	struct rbtree_container *this = NULL;

	node = sta_rbtree->rb_root.rb_node;
	while (node)
	{
		this = rb_entry(node, struct rbtree_container, rb_node);
		result = sta_rbtree->compare_rbtree_struct_cb(user_data, this->rbtree_data);

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
int container_insert(struct rbtree_container_head *sta_rbtree, void *user_data, int rbtree_data_size)
{
	int result = 0;
	struct rb_node *parent = 0;
	struct rbtree_container *rbtree_new = NULL, *this = NULL;
	struct rb_node **new = &sta_rbtree->rb_root.rb_node;

	if(rbtree_data_size <= 0) {
		wpa_printf(MSG_DEBUG, "rbtree_data_siz <= 0");
		return -1;
	}

    rbtree_new = (struct rbtree_container*)malloc(sizeof(struct rbtree_container) + rbtree_data_size);
	if(rbtree_new == NULL) {
		wpa_printf(MSG_DEBUG, "malloc rbtree_new error");
		return -1;
	}

	memcpy((void *)rbtree_new + sizeof(struct rbtree_container), user_data, rbtree_data_size);
	rbtree_new->rbtree_data = (void *)rbtree_new + sizeof(struct rbtree_container);

	rbtree_lock(sta_rbtree);

	while(*new)
	{
		this = rb_entry(*new, struct rbtree_container, rb_node);

		result = sta_rbtree->compare_rbtree_struct_cb(user_data, this->rbtree_data);

		parent = &this->rb_node;

		if (result < 0)
			new = &((*new)->rb_left);
		else if (result > 0)
			new = &((*new)->rb_right);
		else {
			rbtree_unlock(sta_rbtree);
			free(rbtree_new);
			wpa_printf(MSG_DEBUG, "this date is exists");
			return -1;
		}
	}

	/* Add new node and rebalance tree. */
	rb_link_node(&(rbtree_new->rb_node), parent, new);
	rb_insert_color(&(rbtree_new->rb_node), &sta_rbtree->rb_root);

	rbtree_unlock(sta_rbtree);
	return 0;
}

/*
void  container_walk(struct rbtree_container_head *sta_rbtree)
{
	struct rb_node *tmp = NULL;
	struct rbtree_container  *this = NULL;

    if(sta_rbtree == NULL)
        return -1;

   tmp = rb_first(&(sta_rbtree->rb_root));

    while(tmp)
    {
        this = rb_entry(tmp, struct rbtree_container, rb_node);
        display((struct cc*)(this->rbtree_data));

        tmp =rb_next(tmp);
    }
}
*/

/*
* Delete the key node from rbtree
*     delete node from rbtree, return node pointer
*/
int container_delete(struct rbtree_container_head *sta_rbtree, void *user_data)
{
	struct rbtree_container *rbtree_mem = NULL;

	rbtree_lock(sta_rbtree);

	rbtree_mem= container_search2(sta_rbtree, user_data);
	if (!rbtree_mem) {
		rbtree_unlock(sta_rbtree);
		wpa_printf(MSG_DEBUG, "this data is not exists");
		return 0;
	}

	rb_erase(&rbtree_mem->rb_node, &sta_rbtree->rb_root);

	rbtree_unlock(sta_rbtree);
	free(rbtree_mem);
	return 0;
}

void container_flush(struct rbtree_container_head *sta_rbtree)
{
	int count = 0;
	struct rb_node *node = NULL;
	struct rbtree_container *this = NULL;

	node = sta_rbtree->rb_root.rb_node;

	rbtree_lock(sta_rbtree);
	while(node)
	{
		this = rb_entry(node, struct rbtree_container, rb_node);
		rb_erase(&this->rb_node, &sta_rbtree->rb_root);
		free(this);
		node = sta_rbtree->rb_root.rb_node;
		count++;
	}
	rbtree_unlock(sta_rbtree);

	wpa_printf(MSG_DEBUG, "container flush success :%d count data!", count);
	return ;
}
