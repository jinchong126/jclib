#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "list.h"
#include "rbtree.h"
#include "timerqueue.h"

void *malloc_node()
{
	struct timerqueue_node *node = NULL;
	node = (struct timerqueue_node *)malloc(sizeof(struct timerqueue_node));
	if(node == NULL) {
		printf("malloc error \n");
	}
	return node;
}

int timerqueue_print(struct timerqueue_node *node)
{
	printf("print val:%lld\n", node->expires);
	return 0;
}

int main()
{
	int i, n = 50;
	struct timerqueue_head timerh;
	struct timerqueue_node *node = NULL;

	/* init */
	timerqueue_init_head(&timerh);

	srand((int)time(0));

	/* add */
	printf("add timer-----------------------\n");
	for(i = 0; i < n; i++){
		node = malloc_node();
		node->expires = rand() % 1000;
		timerqueue_add(&timerh, node);

		node = timerqueue_getnext(&timerh);
		printf("---------------------------number one val %lld\n", node->expires);
	}
	printf("lookup timer -----------------------\n");

	/* lookup */
	node = NULL;
	do{
	node = timerqueue_next(&timerh, node);
	if(node != NULL)
	{
		printf("expires val:%lld\n", node->expires);
	} else {
		printf("no expires val\n");
	}
	}while(node);

	printf("del timer-----------------------\n");
	/* delete */
	node = timerqueue_getnext(&timerh);
	timerqueue_del(&timerh, node);
	free(node);

	/* lookup */
	printf("lookup timer -----------------------\n");
	node = NULL;
	do{
	node = timerqueue_next(&timerh, node);
	//node = timerqueue_getnext(&timerh);
	if(node != NULL)
	{
		printf("expires val:%lld\n", node->expires);
	} else {
		printf("no expires val\n");
	}
	}while(node);

	printf("each timer -----------------------\n");
	timerqueue_each_node(&timerh, timerqueue_print);

	return 0;
}
