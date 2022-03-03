#ifndef __HASH_LIST__
#define __HASH_LIST__

#include "list.h"

#ifndef BASE_HASH_TABLE_MAX
#define BASE_HASH_TABLE_MAX  (16)
#endif

typedef unsigned int (*base_hash_val_cb)(void *user_struct);
typedef int (*base_hash_compare_cb)(void *user_struct, void *mem_struct);

/* hash table head */
struct base_hash_table_head
{
	struct hlist_head hash_table_head[BASE_HASH_TABLE_MAX];
	spinlock_t hash_table_lock;
};

/* hash table list */
struct base_hash_table_list
{
	void *struct_head;
	struct hlist_node list;
};

/* hash table callback function*/
struct base_hash_table_list_cb
{
	base_hash_val_cb calculate_hash_val_cb;
	base_hash_compare_cb compare_struct_cb;
};

/* *****************************************************************************
 *     Function:       init_lock_hash_table
 *     Description:    初始化hash表锁
 *     Input:
 *			hash_val_cb 		--hash值用户实现回调函数
 *			compare_struct_cb 	--用户定义结构化数据比较函数(1-比较成功,0-比较失败)
 *			hash_table_size 	--用户定义hash表大小
 *     Output:         
 *     Return:        
 *     Others:      	<其它说明>
 *******************************************************************************/
int init_base_hash_table(base_hash_val_cb hash_val_cb, base_hash_compare_cb compare_struct_cb, unsigned int hash_table_size);


/* *****************************************************************************
 *     Function:       add_base_hash_table
 *     Description:    增加数据
 *     Input:
 *			user_struct --用户结构体
 *			struct_size --结构体大小
 *     Output:         
 *     Return:        
 *     Others:      	<其它说明>
 *******************************************************************************/
int add_base_hash_table(void *user_struct, int struct_size);

/* *****************************************************************************
 *     Function:       del_base_hash_table
 *     Description:    删除数据
 *     Input:
 *			user_struct --用户结构体
 *     Output:         
 *     Return:        
 *     Others:      	<其它说明>
 *******************************************************************************/
int del_base_hash_table(void *user_struct);

/* *****************************************************************************
 *     Function:       flush_base_hash_table
 *     Description:    清楚hash链表
 *     Input:
 *     Output:         
 *     Return:        
 *     Others:      	<其它说明>
 *******************************************************************************/
void flush_base_hash_table(void);

/* *****************************************************************************
 *     Function:       lookup_user_struct_by_hash_table
 *     Description:    根据用户结构关键字段查找数据
 *     Input:
 *			user_struct --用户结构体
 *     Output:         
 *     Return:        
 *     Others:      	<其它说明>
 *******************************************************************************/
void *lookup_user_struct_by_hash_table(void *user_struct);

#endif 
