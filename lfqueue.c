/* Thread support only 1-to-1 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lfqueue.h"

struct lock_free_queue_node *g_lf_queue_head; /* queue head */
struct lock_free_queue_node *g_lf_queue_tail; /* queue tail */

/* init lock-free queue */
int lf_queue_init()
{
	g_lf_queue_tail = NULL;
	g_lf_queue_head = NULL;

	g_lf_queue_head = (struct lock_free_queue_node*)malloc(sizeof(struct lock_free_queue_node));
	if (g_lf_queue_head == NULL)
	{
		printf("Malloc lock-free queue node failure, Init error\n");
		return -1;
	}

	/* set head and tail is not null */
	g_lf_queue_head->data = NULL;
	g_lf_queue_head->next = NULL;
	g_lf_queue_tail = g_lf_queue_head;
	return 0;
}

/* lock-free queue push packet */
int lf_queue_push(void *pdata)
{
	/* init */
	struct lock_free_queue_node* node  = NULL;
	struct lock_free_queue_node* tmp  = NULL;

	node = (struct lock_free_queue_node*)malloc(sizeof(struct lock_free_queue_node));
	if (node == NULL)
	{
		printf("Malloc lock-free queue node failure\n");
		return -1;
	}

	/* write data */
	node->data = pdata;
	node->next = NULL;

	/* inset tail */
	tmp = g_lf_queue_tail;
	g_lf_queue_tail = node;
	tmp->next = node;
	return 0;
}

/* lock-free queue push packet */
void *lf_queue_pop()
{
	/* init result */
	void *result = NULL;
	struct lock_free_queue_node *node = g_lf_queue_head, *new;

	/* get data */
	if (g_lf_queue_head->next != NULL)
	{
		g_lf_queue_head = g_lf_queue_head->next;
		result = node->data;

		/* free node */
		free(node);
	} else {
		if(g_lf_queue_head->data != NULL) {
			new = (struct lock_free_queue_node*)malloc(sizeof(struct lock_free_queue_node));
			if (new == NULL)
			{
				printf("Malloc lock-free queue node failure, Init error\n");
				return NULL;
			}

			new->data = NULL;
			new->next = NULL;
			/* get tail data and put null node */
			g_lf_queue_head = new;
			g_lf_queue_tail = new;
			result = node->data;
			free(node);
		}
	}

	return result;
}
