#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

struct list_test{
	int i;
	char val[100];
	struct dl_list list;
};

int main()
{
	struct list_test test;
	struct list_test *t;

	dl_list_init(&test.list);

	t = malloc(sizeof(struct list_test));
	if(t == NULL) {
		printf("malloc error\n");
		exit(0);
	}

	t->i = 1;
	strcpy(t->val, "test1");

	printf("add list-----------------------\n");
	dl_list_add_tail(&test.list, &t->list);

	t = malloc(sizeof(struct list_test));
	if(t == NULL) {
		printf("malloc error\n");
		exit(0);
	}

	t->i = 2;
	strcpy(t->val, "test2");

	printf("add list-----------------------\n");
	dl_list_add_tail(&test.list, &t->list);

	printf("lookup first list -----------------------\n");
	t = dl_list_first(&test.list, struct list_test, list);
	if(t != NULL)
	{
		printf("xxx hash val:%s\n", t->val);
	}

	printf("del list-----------------------\n");
	dl_list_del(&t->list);
	free(t);

	printf("lookup list -----------------------\n");
	t = dl_list_first(&test.list, struct list_test, list);
	if(t != NULL)
	{
		printf("xxx hash val:%s\n", t->val);
	}
}
