#include "list.h"
#include "base.h"
#include "rbtree_com.h"

/*******************************************************************************
 *     Function:       rbtree_lock_init
 *     Description:    红黑数锁初始化
 *     Input:
 *     Output:         
 *     Return:        
 *     Others:          <其它说明>
 *******************************************************************************/
static inline int rbtree_lock_init(struct rbtree_container_root *rbcr)
{
	pthread_spin_init(&(rbcr->rbtree_container_lock), 0);
	return 0;
}

static inline int rbtree_lock(struct rbtree_container_root *rbcr)
{
	pthread_spin_lock(&(rbcr->rbtree_container_lock));
	return 0;
}

static inline int rbtree_unlock(struct rbtree_container_root *rbcr)
{
	pthread_spin_unlock(&(rbcr->rbtree_container_lock));
	return 0;
}

/*
* Initializing rbtree root
*/
struct rbtree_container_root *rbtree_contarner_init(rbtree_compare_cb compare_rbtree_cb)
{
    struct rbtree_container_root *rbcr;

	if(compare_rbtree_cb == NULL) {
		log_printf(MSG_ERROR, "rbtree call back function is null!\n");
		return NULL;
	}

	rbcr = (struct rbtree_container_root *)malloc(sizeof(struct rbtree_container_root));
	if (rbcr == NULL) {
		log_printf(MSG_ERROR, "Malloc rbcr failure, Init error\n");
		return NULL;
    }


	memset(rbcr, 0, sizeof(struct rbtree_container_root));
	rbcr->rb_root = RB_ROOT;
	rbcr->compare_rbtree_struct_cb = compare_rbtree_cb;

	rbtree_lock_init(rbcr);
	log_printf(MSG_DEBUG, "rbtree contarner init completed!\n");
	return rbcr;
}

void container_deinit(struct rbtree_container_root *rbcr)
{
    if (rbcr == NULL) {
        return;
    }

    container_flush(rbcr);

    free(rbcr);

    return;
}

/*
* Search key node from rbtree
*/
void *container_search(struct rbtree_container_root *rbcr, void *user_data)
{
	int result;
	struct rb_node *node = rbcr->rb_root.rb_node;

	while (node)
	{
		struct rbtree_container *this = rb_entry(node, struct rbtree_container, rb_node);

		result = rbcr->compare_rbtree_struct_cb(user_data, this->rbtree_data);
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

void *container_search2(struct rbtree_container_root *rbcr, void *user_data)
{
	int result;
	struct rb_node *node = NULL;
	struct rbtree_container *this = NULL;

	node = rbcr->rb_root.rb_node;
	while (node)
	{
		this = rb_entry(node, struct rbtree_container, rb_node);
		result = rbcr->compare_rbtree_struct_cb(user_data, this->rbtree_data);

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
int container_insert(struct rbtree_container_root *rbcr, void *user_data, int rbtree_data_size)
{
	int result = 0;
	struct rb_node *parent = 0;
	struct rbtree_container *rbtree_new = NULL, *this = NULL;
	struct rb_node **new = &(rbcr->rb_root.rb_node);

	if(rbtree_data_size <= 0) {
		log_printf(MSG_DEBUG, "rbtree_data_siz <= 0\n");
		return -1;
	}

    rbtree_new = (struct rbtree_container*)malloc(sizeof(struct rbtree_container) + rbtree_data_size);
	if(rbtree_new == NULL) {
		log_printf(MSG_DEBUG, "malloc rbtree_new error\n");
		return -1;
	}

	memcpy((void *)rbtree_new + sizeof(struct rbtree_container), user_data, rbtree_data_size);
	rbtree_new->rbtree_data = (void *)rbtree_new + sizeof(struct rbtree_container);

	rbtree_lock(rbcr);

	while(*new)
	{
		this = rb_entry(*new, struct rbtree_container, rb_node);

		result = rbcr->compare_rbtree_struct_cb(user_data, this->rbtree_data);

		parent = &this->rb_node;

		if (result < 0)
			new = &((*new)->rb_left);
		else if (result > 0)
			new = &((*new)->rb_right);
		else {
			rbtree_unlock(rbcr);
			free(rbtree_new);
			log_printf(MSG_DEBUG, "this date is exists\n");
			return -1;
		}
	}

	/* Add new node and rebalance tree. */
	rb_link_node(&(rbtree_new->rb_node), parent, new);
	rb_insert_color(&(rbtree_new->rb_node), &rbcr->rb_root);

	rbtree_unlock(rbcr);
	return 0;
}

/*
* Delete the key node from rbtree
*     delete node from rbtree, return node pointer
*/
int container_delete(struct rbtree_container_root *rbcr, void *user_data)
{
	struct rbtree_container *rbtree_mem = NULL;

	rbtree_lock(rbcr);

	rbtree_mem= container_search2(rbcr, user_data);
	if (!rbtree_mem) {
		rbtree_unlock(rbcr);
		log_printf(MSG_DEBUG, "this data is not exists\n");
		return 0;
	}

	rb_erase(&rbtree_mem->rb_node, &rbcr->rb_root);

	rbtree_unlock(rbcr);
	free(rbtree_mem);
	return 0;
}

void container_flush(struct rbtree_container_root *rbcr)
{
	int count = 0;
	struct rb_node *node = NULL;
	struct rbtree_container *this = NULL;

	node = rbcr->rb_root.rb_node;

	rbtree_lock(rbcr);
	while(node)
	{
		this = rb_entry(node, struct rbtree_container, rb_node);
		rb_erase(&this->rb_node, &rbcr->rb_root);
		free(this);
		node = rbcr->rb_root.rb_node;
		count++;
	}
	rbtree_unlock(rbcr);

	log_printf(MSG_DEBUG, "container flush success :%d count data!\n", count);
	return ;
}
