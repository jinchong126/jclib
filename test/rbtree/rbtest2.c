#include "rbtree_rc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int
main(void)
{
	struct rb_root mytree;
	struct container * pc;
	int ret;
	int size = sizeof(int);
	int find, erase, replace;
	
	const int max = 1000000;
	int i, rd;
	
	printf("rbtest go ... \n");
	
	init_rbtree(&mytree);
	
	for (i = 0; i < max; ++i)
	{
		rd = rand() % max;
		
		// container 创建
		pc = calloc(sizeof(struct container) + size, 1);
		pc -> rb_data.key = i;
		*(int*)&pc -> rb_data.data = rd;
		pc -> rb_data.size = size;
		
		// 插入
		ret = container_insert(&mytree, pc);		
		if (ret < 0)
			printf("failed + insert key: %d, data: %d\n", i, rd);
		else
			printf("successful + insert  key: %d, data: %d\n", i, rd);
		
		// 查找			
		pc = container_search(&mytree, i);
		if (pc != 0)
			printf("successful ^ search  key: %d, data: %d\n", i, *(int*)&pc->rb_data.data);
		else
			printf("failed ^ search no key %d\n", i);
		
		// 替换某些节点
		if (i % 20 == 0)
		{
			pc = calloc(sizeof(struct container) + size, 1);
			pc -> rb_data.key = i;
			*(int*)&pc -> rb_data.data = rand() % max;
			pc -> rb_data.size = size;
			pc = container_replace(&mytree, i, pc);
			if (pc != 0)
			{
				printf("successful @ replace key: %d, old data: %d, new data: %d\n", 
						i, *(int*)&pc->rb_data.data, 
						*(int*)&container_search(&mytree, i)->rb_data.data
						);
				free(pc);
			}
			else
			{
				printf("failed @ replace key: %d failed\n", i);	
			}	
		}
		
		// 删除某些节点
		if (i % 10 == 0)
		{
			pc = container_delete(&mytree, i);
			if (pc == 0)
			{
				printf("failed - delete key: %d\n", i);
			}
			else
			{
				printf("successful - delete  key: %d, data: %d\n", i, *(int*)&pc->rb_data.data);
				free(pc);
			}
		}
	}

	printf("rbtest end\n");
		
	return 0;
}
