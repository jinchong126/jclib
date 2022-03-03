#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_list.h"

struct hash_test{
	int i;
	char val[100];
};

static unsigned int calc_hash_val(void *hash)
{
	struct hash_test *tmp;
	tmp = (struct hash_test *)hash;
	return (unsigned int)(tmp->i%10);
}


int cmp_hash_val(void *hash, void *mem_hash)
{
	struct hash_test *tmp, *tmp2;
	tmp = (struct hash_test *)hash;
	tmp2 = (struct hash_test *)mem_hash;
	if(tmp->i == tmp2->i)
		return 1;
	else
		return 0;
}

struct hash_table_opt hash_opt = {
	.name = "hash_xx_test",
	.hash_size = 10,
	.need_lock = HASH_NEED_LOCK_MUTEX,
	.hash_val_cb = calc_hash_val,
	.cmp_cb = cmp_hash_val,
};

int hash_table_print(void *t1, void *arg)
{
	struct hash_test *t = t1;
	printf("cb print i %d val %s\n", t->i, t->val);
	return 0;
}

int add_test()
{
	int i = 0;
	struct hash_test test;


	for (; i < 40; i++)
	{
		memset(&test, 0, sizeof(struct hash_test));
		if(i > 20)
			test.i = i%20;
		else
			test.i = i;
		sprintf(test.val, "test%d", i);
		hash_table_reusable_add(&hash_opt, (void *)&test, sizeof(struct hash_test));
	}
	return 0;
}

int main()
{
	struct hash_test *t = NULL;
	struct hash_test test;

	if(hash_table_init(&hash_opt) < 0) {
		printf("init error\n");
		exit(0);
	}

	printf("add hash table-----------------------\n");
	add_test();
	printf("add hash table end-----------------------\n");


	printf("each node table-----------------------\n");
	hash_table_each_node(&hash_opt, hash_table_print, NULL);
	printf("each node table end -----------------------\n");

	printf("del hash table-----------------------\n");
	test.i = 15;
	hash_table_reusable_del(&hash_opt, (void *)&test);

/*
	printf("walk hash begin=============================================\n");
	{
		struct hash_test *tt = NULL;
		while(1) {
			tt = hash_table_next_by_user(&hash_opt, tt);
			if(tt != NULL)
			{
				printf("xxx hash val:%s\n", tt->val);
			} else {
				break;
			}
		}
	}
	printf("walk hash end =============================================\n");
*/


	printf("each node table-----------------------\n");
	hash_table_each_node(&hash_opt, hash_table_print, NULL);
	printf("lookup hash table-----------------------\n");
	t = hash_table_lookup_user(&hash_opt, (void *)&test);
	if(t != NULL)
	{
		printf("xxx hash val:%s\n", t->val);
	}

	printf("flush hash table-----------------------\n");
	hash_table_flush(&hash_opt);
	return 0;
}
