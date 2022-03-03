#include <linux/slab.h>
#include "hash_list.h"

static struct base_hash_table_head g_base_hash_table;
static struct base_hash_table_list_cb g_base_hash_table_cb;
static unsigned int g_base_hash_table_size;

/*******************************************************************************
 *     Function:       init_lock_hash_table
 *     Description:    
 *     Input:
 *     Output:         
 *     Return:        
 *     Others:      
 *******************************************************************************/
static inline int init_lock_hash_table(void)
{
	spin_lock_init(&(g_base_hash_table.hash_table_lock));
	return 0;
}

static inline int lock_hash_table(void)
{
	spin_lock_irq(&(g_base_hash_table.hash_table_lock));
	return 0;
}

static inline int unlock_hash_table(void)
{
	spin_unlock_irq(&(g_base_hash_table.hash_table_lock));
	return 0;
}

/* *****************************************************************************
 *     Function:       init_lock_hash_table
 *     Description:   
 *     Input:
 *			hash_val_cb 		--
 *			compare_struct_cb 	--
 *			hash_table_size 	--
 *     Output:         
 *     Return:        
 *     Others:      	
 *******************************************************************************/
int init_base_hash_table(base_hash_val_cb hash_val_cb, base_hash_compare_cb compare_struct_cb, unsigned int hash_table_size)
{
	unsigned int i = 0;
	if(hash_val_cb == NULL || compare_struct_cb == NULL) {
		printk("hash table function is null\n");
		return -1;
	}

	if(hash_table_size <= 0 || hash_table_size > BASE_HASH_TABLE_MAX) {
		printk("hash table size [%d] error [0 - %d]\n", hash_table_size, BASE_HASH_TABLE_MAX);
		return -1;
	}

	g_base_hash_table_cb.calculate_hash_val_cb = hash_val_cb;
	g_base_hash_table_cb.compare_struct_cb = compare_struct_cb;
	g_base_hash_table_size = hash_table_size;

	for(; i < g_base_hash_table_size; i++){
		g_base_hash_table.hash_table_head[i].first = NULL;
	}

	init_lock_hash_table();
	printk("init hash table complete\n");
	return 0;
}

/* *****************************************************************************
 *     Function:       lookup_base_hash_table
 *     Description:    
 *     Input:
 *			user_struct --
 *     Output:         
 *     Return:        
 *     Others:      	<>
 *******************************************************************************/
struct base_hash_table_list *lookup_base_hash_table(void *user_struct)
{
	unsigned int hash_val = 0;
	struct base_hash_table_list *p_base_data = NULL;
	struct hlist_node *tmp_node = NULL, *safe_tmp_node = NULL;

	hash_val = (unsigned int)g_base_hash_table_cb.calculate_hash_val_cb(user_struct);
	printk("hash value=%d\n",hash_val);

	hlist_for_each_entry_safe(p_base_data, tmp_node, safe_tmp_node, \
				&(g_base_hash_table.hash_table_head[hash_val]), list){
		if(g_base_hash_table_cb.compare_struct_cb(user_struct, p_base_data->struct_head)) {
			printk("select ok\n");
			return p_base_data;
		}
	}
	printk("select NULL\n");
	return NULL;
}

/* *****************************************************************************
 *     Function:       add_base_hash_table
 *     Description:    
 *     Input:
 *			user_struct --
 *			struct_size --
 *     Output:         
 *     Return:        
 *     Others:      	<>
 *******************************************************************************/
int add_base_hash_table(void *user_struct, int struct_size)
{
	unsigned int hash_val = 0;
	struct base_hash_table_list *hash_list = NULL;

	lock_hash_table();
	if(NULL != lookup_base_hash_table(user_struct)){
		unlock_hash_table();
		printk("this hash table is found\n");
		kfree(hash_list);
		return -1;
	}

	hash_val = (unsigned int)g_base_hash_table_cb.calculate_hash_val_cb(user_struct);

	hash_list = (struct base_hash_table_list *)kmalloc(sizeof(struct base_hash_table_list) + struct_size, GFP_ATOMIC);
	if(hash_list == NULL) {
		printk("malloc base_hash_table_list error\n");
		return -1;
	}
	hash_list->struct_head = ((void *)hash_list) + sizeof(struct base_hash_table_list);
	memcpy(hash_list->struct_head, user_struct, struct_size);

	hlist_add_head(&(hash_list->list), &(g_base_hash_table.hash_table_head[hash_val]));
	unlock_hash_table();
	return 0;
}

/* *****************************************************************************
 *     Function:       del_base_hash_table
 *     Description:    
 *     Input:
 *			user_struct --
 *     Output:         
 *     Return:        
 *     Others:      	<>
 *******************************************************************************/
int del_base_hash_table(void *user_struct)
{
	struct base_hash_table_list *hash_list = NULL;

	lock_hash_table();
	if(NULL == (hash_list = lookup_base_hash_table(user_struct))) {
		unlock_hash_table();
		printk("not found hash table\n");
		return -1;
	}
	hlist_del(&(hash_list->list));
	unlock_hash_table();

	kfree(hash_list);
	return 0;
}

/* *****************************************************************************
 *     Function:       flush_base_hash_table
 *     Description:    
 *     Input:
 *     Output:         
 *     Return:        
 *     Others:      	<>
 *******************************************************************************/
void flush_base_hash_table()
{
	unsigned int i = 0;
	struct hlist_node  *tmp_node=NULL;
	struct base_hash_table_list *p_node=NULL;

	lock_hash_table();

	for(; i < g_base_hash_table_size; i++){
		while(!hlist_empty(&(g_base_hash_table.hash_table_head[i]))){
			tmp_node = (g_base_hash_table.hash_table_head[i]).first;
			if(NULL != tmp_node){
				hlist_del((g_base_hash_table.hash_table_head[i]).first);
				p_node = hlist_entry(tmp_node, struct base_hash_table_list, list);
				kfree(p_node);
			}
		}
	}

	unlock_hash_table();
	return ;
}

/* *****************************************************************************
 *     Function:       lookup_user_struct_by_hash_table
 *     Description:    
 *     Input:
 *			user_struct --用户结构体
 *     Output:         
 *     Return:        
 *     Others:      	<>
 *******************************************************************************/
void *lookup_user_struct_by_hash_table(void *user_struct)
{
	struct base_hash_table_list *hash_list = NULL;

	if(NULL == (hash_list = lookup_base_hash_table(user_struct))) {
		printk("not found hash table\n");
		return NULL;
	}else{
		return hash_list->struct_head;
	}
}
