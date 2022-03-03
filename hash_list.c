#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hash_list.h"
//#include "../debug_msg.h"

/*******************************************************************************
 *     Function:       hash_table_init_lock
 *     Description:    
 *     Input:
 *     Output:         
 *     Return:        
 *     Others:      
 *******************************************************************************/
static inline int hash_table_init_lock(struct hash_table_opt *opt)
{
	struct base_hash_table_head *head;

	if(!opt)
		return -1;

	head = &(opt->hdr);
	switch(opt->need_lock)
	{
	case HASH_NEED_LOCK_NONE:
		break;
	case HASH_NEED_LOCK_SPIN:
		pthread_spin_init(&(head->hash_lock_spin), 0);
		break;
	case HASH_NEED_LOCK_MUTEX:
		pthread_mutex_init(&(head->hash_lock_mutex), NULL);
		break;
	default:
		return -1;
	}
	return 0;
}

static inline int hash_table_lock(struct hash_table_opt *opt)
{
	struct base_hash_table_head *head;

	if(!opt)
		return -1;

	head = &(opt->hdr);
	switch(opt->need_lock)
	{
	case HASH_NEED_LOCK_NONE:
		break;
	case HASH_NEED_LOCK_SPIN:
		pthread_spin_lock(&(head->hash_lock_spin));
		break;
	case HASH_NEED_LOCK_MUTEX:
		pthread_mutex_lock(&(head->hash_lock_mutex));
		break;
	default:
		return -1;
	}
	return 0;
}

static inline int hash_table_unlock(struct hash_table_opt *opt)
{
	struct base_hash_table_head *head;

	if(!opt)
		return -1;

	head = &(opt->hdr);
	switch(opt->need_lock)
	{
	case HASH_NEED_LOCK_NONE:
		break;
	case HASH_NEED_LOCK_SPIN:
		pthread_spin_unlock(&(head->hash_lock_spin));
		break;
	case HASH_NEED_LOCK_MUTEX:
		pthread_mutex_unlock(&(head->hash_lock_mutex));
		break;
	default:
		return -1;
	}
	return 0;
}

/* *****************************************************************************
 *     Function:       hash_table_init
 *     Description:    
 *     Input:
 *     			opt		--hash_table_opt
 *     Output:         
 *     Return:        
 *     Others:      	
 *******************************************************************************/
int hash_table_init(struct hash_table_opt *opt)
{
	int i = 0;
	struct base_hash_table_head *hdr = NULL;

	if(!opt)
		return -1;

	if(opt == NULL) {
		//DB_ERR("hash table opt is null");
		return -1;
	}

	if(opt->hash_val_cb == NULL || opt->cmp_cb == NULL) {
		//DB_ERR("hash table transaction function is null");
		return -1;
	}

	if(opt->hash_size > BASE_HASH_TABLE_MAX) {
		//DB_ERR("hash table size [%d] error [0 - %d]", opt->hash_size, BASE_HASH_TABLE_MAX);
		return -1;
	}
	hdr = &(opt->hdr);
	hdr->hash_table_head = NULL;
	hdr->hash_table_head = malloc(opt->hash_size * sizeof(struct hlist_head));
	if(hdr->hash_table_head == NULL) {
		//DB_ERR("hash table head malloc error n");
		return -1;
	}

	for(; i < opt->hash_size; i++) {
		hdr->hash_table_head[i].first = NULL;
	}

	hash_table_init_lock(opt);
	//DB_DEBUG("init hash table %s is completed", opt->name);
	return 0;
}

/*******************************************************************************
 *     Function:       hash_table_lookup_user
 *     Description:    
 *     Input:
 *     			opt		--hash_table_opt
 *			user_struct	--
 *     Output:         
 *     Return:        
 *     Others:      	
 *******************************************************************************/
void *hash_table_lookup_user(struct hash_table_opt *opt, void *user_struct)
{
	unsigned int hash_val = 0;
	struct base_hash_table_list *p_base_data = NULL;
	struct hlist_node *tmp_node = NULL, *safe_tmp_node = NULL;

	if(!opt || !user_struct)
		return NULL;

	hash_val = (unsigned int)opt->hash_val_cb(user_struct);
	//DB_DEBUG("hash value=%d",hash_val);

	hlist_for_each_entry_safe(p_base_data, tmp_node, safe_tmp_node, \
		(opt->hdr.hash_table_head + hash_val), list){
		if(opt->cmp_cb(user_struct, p_base_data->struct_head)) {
			//DB_DEBUG("select ok");
			return p_base_data->struct_head;
		}
	}
	//DB_DEBUG("select NULL");
	return NULL;
}

/*******************************************************************************
 *     Function:       hash_table_lookup
 *     Description:    
 *     Input:
 *     			opt		--hash_table_opt
 *			user_struct	--
 *     Output:         
 *     Return:        
 *     Others:      	
 *******************************************************************************/
struct base_hash_table_list *hash_table_lookup(struct hash_table_opt *opt, void *user_struct)
{
	unsigned int hash_val = 0;
	struct base_hash_table_list *p_base_data = NULL;
	struct hlist_node *tmp_node = NULL, *safe_tmp_node = NULL;

	if(!opt || !user_struct)
		return NULL;

	hash_val = (unsigned int)opt->hash_val_cb(user_struct);
	//DB_DEBUG("hash value=%d",hash_val);

	hlist_for_each_entry_safe(p_base_data, tmp_node, safe_tmp_node, \
		(opt->hdr.hash_table_head + hash_val), list){
		if(opt->cmp_cb(user_struct, p_base_data->struct_head)) {
			//DB_DEBUG("select ok");
			return p_base_data;
		}
	}
	//DB_DEBUG("select NULL");
	return NULL;
}

/* *****************************************************************************
 *     Function:       hash_table_add
 *     Description:    
 *     Input:
 *     			opt		--hash_table_opt
 *			user_struct --
 *			struct_size --
 *     Output:         
 *     Return:        
 *     Others:      	
 *******************************************************************************/
void *hash_table_add(struct hash_table_opt *opt, void *user_struct, int struct_size)
{
	unsigned int hash_val = 0;
	void *p;
	struct base_hash_table_list *hash_list = NULL;

	if(!opt || !user_struct || struct_size <= 0)
		return NULL;

	hash_val = (unsigned int)opt->hash_val_cb(user_struct);

	p = malloc(sizeof(struct base_hash_table_list) + struct_size);
	if(p == NULL) {
		//DB_ERR("malloc base_hash_table_list error");
		return NULL;
	}
	//根据实际情况可以外部赋值
	memcpy(p, user_struct, struct_size);

	hash_list = (struct base_hash_table_list *)(p + struct_size);
	hash_list->struct_head = p;

	hash_table_lock(opt);
	if(NULL != hash_table_lookup(opt, user_struct)){
		hash_table_unlock(opt);
		//DB_DEBUG("this hash table is found");
		free(p);
		return NULL;
	}

	hlist_add_head(&(hash_list->list), (opt->hdr.hash_table_head + hash_val));
	hash_table_unlock(opt);
	return hash_list->struct_head;
}

/* *****************************************************************************
 *     Function:       hash_table_del
 *     Description:    
 *     Input:
 *     			opt		--hash_table_opt
 *			user_struct --
 *     Output:         
 *     Return:        
 *     Others:      	
 *******************************************************************************/
int hash_table_del(struct hash_table_opt *opt, void *user_struct)
{
	struct base_hash_table_list *hash_list = NULL;

	if(!opt || !user_struct)
		return -1;

	hash_table_lock(opt);
	if(NULL == (hash_list = hash_table_lookup(opt, user_struct))) {
		hash_table_unlock(opt);
		//DB_ERR("not found hash table");
		return -1;
	}
	hlist_del(&(hash_list->list));
	hash_table_unlock(opt);

	free(hash_list->struct_head);
	
	return 0;
}

/* *****************************************************************************
 *     Function:       hash_table_del_user_free
 *     Description:    
 *     Input:
 *     			opt		--hash_table_opt
 *			user_struct --
 *     Output:         
 *     Return:        
 *     Others:      	
 *******************************************************************************/
int hash_table_del_by_addr(struct hash_table_opt *opt, void *user_struct, int size)
{
	struct base_hash_table_list *hash_list = NULL;

	if(!opt || !user_struct)
		return -1;

	hash_list = user_struct + size;
	hash_table_lock(opt);
	hlist_del(&(hash_list->list));
	hash_table_unlock(opt);
	free(hash_list->struct_head);

	return 0;
}

/* *****************************************************************************
 *     Function:       hash_table_flush
 *     Description:    
 *     Input:
 *     			opt		--hash_table_opt
 *     Output:         
 *     Return:        
 *     Others:      
 *******************************************************************************/
void hash_table_flush(struct hash_table_opt *opt)
{
	int i = 0;
	struct hlist_node  *tmp_node=NULL;
	struct base_hash_table_list *p_node=NULL;

	if(!opt)
		return ;

	hash_table_lock(opt);

	for(; i < opt->hash_size; i++){
		while(!hlist_empty((opt->hdr.hash_table_head + i))){
			tmp_node = ((struct hlist_head *)(opt->hdr.hash_table_head + i))->first;
			if(NULL != tmp_node){
				hlist_del((opt->hdr.hash_table_head + i)->first);
				p_node = hlist_entry(tmp_node, struct base_hash_table_list, list);
				free(p_node->struct_head);
			}
		}
	}
	if(opt->hdr.hash_table_head != NULL) {
		free(opt->hdr.hash_table_head);
	}

	hash_table_unlock(opt);
	return ;
}

/* *****************************************************************************
 *     Function:       hash_table_next_by_user
 *     Description:    
 *     Input:
 *     			opt		--hash_table_opt
 *			user_struct	--
 *     Output:         
 *     Return:        
 *     Others:      	
 *******************************************************************************/
void *hash_table_next_by_user(struct hash_table_opt *opt, void *user_struct)
{
	unsigned int hash_val = 0;
	int find = 0, first = 0;
	struct base_hash_table_list *p_base_data = NULL;
	struct hlist_node *tmp_node = NULL, *safe_tmp_node = NULL;

	if(!opt)
		return NULL;

	if(user_struct == NULL) {
		/* lookup first */
		hash_val = 0;
		first = 1;
	} else {
		hash_val = (unsigned int)opt->hash_val_cb(user_struct);
	}
	//DB_DEBUG("hash value=%d",hash_val);

	hash_table_lock(opt);
	while(1) {
		hlist_for_each_entry_safe(p_base_data, tmp_node, safe_tmp_node, \
			(opt->hdr.hash_table_head + hash_val), list){
			if(first) {
				hash_table_unlock(opt);
				return p_base_data->struct_head;
			} else {
				if(opt->cmp_cb(user_struct, p_base_data->struct_head)) {
					//DB_DEBUG("select ok get next");
					find = 1;
					continue;
				}
				if(find) {
					hash_table_unlock(opt);
					return p_base_data->struct_head;
				}
			}
		}

		hash_val++;
		if(hash_val >= opt->hash_size)
			break;
	}
	hash_table_unlock(opt);
	//DB_DEBUG("select NULL");
	return NULL;
}

/* *****************************************************************************
 *     Function:       hash_table_each_node
 *     Description:    
 *     Input:
 *     			opt		--hash_table_opt
 *			user_struct	--
 *     Output:         
 *     Return:        
 *     Others:      	
 *******************************************************************************/
int hash_table_each_node(struct hash_table_opt *opt, base_hash_echo_node_cb cb, void *arg)
{
	unsigned int hash_val = 0, i = 0;
	struct base_hash_table_list *p_base_data = NULL;
	struct hlist_node *tmp_node = NULL, *safe_tmp_node = NULL;

	if(!opt || !cb)
		return -1;

	hash_table_lock(opt);
	while(1) {
		hlist_for_each_entry_safe(p_base_data, tmp_node, safe_tmp_node, \
			(opt->hdr.hash_table_head + hash_val), list){
			hash_table_unlock(opt);
			i++;
			if(cb(p_base_data->struct_head, arg) == 1)
				break;
			hash_table_lock(opt);
		}

		hash_val++;
		if(hash_val >= opt->hash_size)
			break;
	}
	hash_table_unlock(opt);
	//DB_DEBUG("hash node each num %d", i);
	return 0;
}

/* *****************************************************************************
 *     Function:       hash_table_each_node_by_range
 *     Description:    
 *     Input:
 *     			opt		--hash_table_opt
 *			user_struct	--
 *     Output:         
 *     Return:        
 *     Others:      	
 *******************************************************************************/
int hash_table_each_node_by_range(struct hash_table_opt *opt, int begin, int end, base_hash_echo_node_cb cb, void *arg)
{
	unsigned int hash_val = 0, i = 0;
	struct base_hash_table_list *p_base_data = NULL;
	struct hlist_node *tmp_node = NULL, *safe_tmp_node = NULL;

	if(!opt || !cb)
		return -1;

	if(begin < 0 || end > opt->hash_size) {
		return -1;
	}
	hash_val = begin;

	hash_table_lock(opt);
	while(1) {
		hlist_for_each_entry_safe(p_base_data, tmp_node, safe_tmp_node, \
			(opt->hdr.hash_table_head + hash_val), list){
			hash_table_unlock(opt);
			i++;
			if(cb(p_base_data->struct_head, arg) == 1)
				break;
			hash_table_lock(opt);
		}

		hash_val++;
		if(hash_val >= end)
			break;
	}
	hash_table_unlock(opt);
	//DB_DEBUG("hash node each num %d", i);
	return 0;
}

/* *****************************************************************************
 *     Function:       hash_table_reusable_add
 *     Description:    
 *     Input:
 *     			opt		--hash_table_opt
 *			user_struct --
 *			struct_size --
 *     Output:         
 *     Return:        
 *     Others:      	
 *******************************************************************************/
void *hash_table_reusable_add(struct hash_table_opt *opt, void *user_struct, int struct_size)
{
	unsigned int hash_val = 0;
	void *p = NULL;
	struct base_hash_table_list *hash_list = NULL;

	if(!opt || !user_struct || struct_size <=0 )
		return NULL;

	hash_val = (unsigned int)opt->hash_val_cb(user_struct);

	p = malloc(sizeof(struct base_hash_table_list) + struct_size);
	if(p == NULL) {
		//DB_DEBUG("malloc base_hash_table_list error\n");
		return NULL;
	}

	//根据实际情况可以外部赋值
	memcpy(p, user_struct, struct_size);

	hash_list = (struct base_hash_table_list *)(p + struct_size);

	hash_list->struct_head = p;

	hash_table_lock(opt);
	hlist_add_head(&(hash_list->list), (opt->hdr.hash_table_head + hash_val));
	hash_table_unlock(opt);

	return hash_list->struct_head;
}

/* *****************************************************************************
 *     Function:       hash_table_reusable_del
 *     Description:    
 *     Input:
 *     			opt		--hash_table_opt
 *			user_struct --
 *     Output:         
 *     Return:        
 *     Others:      	
 *******************************************************************************/
int hash_table_reusable_del(struct hash_table_opt *opt, void *user_struct)
{
	unsigned int hash_val = 0;
	int removed = 0;
	struct base_hash_table_list *p_base_data = NULL;
	struct hlist_node *tmp_node = NULL, *safe_tmp_node = NULL;

	if(!opt || !user_struct)
		return -1;

	hash_val = (unsigned int)opt->hash_val_cb(user_struct);
	//DB_DEBUG("Reusable del: hash value=%d line=%d", hash_val, __LINE__);

	hash_table_lock(opt);
	hlist_for_each_entry_safe(p_base_data, tmp_node, safe_tmp_node, \
		(opt->hdr.hash_table_head + hash_val), list){
		if(opt->cmp_cb(user_struct, p_base_data->struct_head)) {
			//DB_DEBUG("Reusable del: ok");
			hlist_del(&(p_base_data->list));
			free(p_base_data->struct_head);
			removed++;
		}
	}
	hash_table_unlock(opt);
	return removed;
}
