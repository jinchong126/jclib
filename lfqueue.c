/* Thread support only 1-to-1 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lfqueue.h"

/* init lock-free queue */
struct lf_opt *lf_queue_init()
{
    struct lf_opt *lf;
	lf = (struct lf_opt *)malloc(sizeof(struct lf_opt));
	if (lf == NULL) {
		printf("Malloc lock-free queue opt failure, Init error\n");
		return NULL;
    }

	lf->head = (struct lock_free_queue_node*)malloc(sizeof(struct lock_free_queue_node));
	if (lf->head == NULL)
	{
		printf("Malloc lock-free queue node failure, Init error\n");
        free(lf);
		return NULL;
	}

	/* set head and tail is not null */
	lf->head->data = NULL;
	lf->head->next = NULL;
	lf->tail = lf->head;
	return 0;
}

/* deinit lock-free queue */
void lf_queue_deinit(struct lf_opt *lf)
{
	struct lock_free_queue_node *n, *p;

    if (lf == NULL) {
        return ;
    }

	n = lf->head;

    while(n != lf->tail) {
        if (n->data != NULL) {
            free(n->data);
            n->data = NULL;
        }
        p = n;
        n = n->next;
        free(p);
    }

    free(lf->head);
    lf->head = NULL;

    free(lf);
    lf = NULL;
	return ;
}

/* lock-free queue push packet */
int lf_queue_push(struct lf_opt *lf, void *pdata)
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
	tmp = lf->tail;
	lf->tail = node;
	tmp->next = node;
	return 0;
}

/* lock-free queue push packet */
void *lf_queue_pop(struct lf_opt *lf)
{
	/* init result */
	void *result = NULL;
	struct lock_free_queue_node *node = lf->head, *new;

	/* get data */
	if (lf->head->next != NULL)
	{
		lf->head = lf->head->next;
		result = node->data;

		/* free node */
		free(node);
	} else {
		if(lf->head->data != NULL) {
			new = (struct lock_free_queue_node*)malloc(sizeof(struct lock_free_queue_node));
			if (new == NULL)
			{
				printf("Malloc lock-free queue node failure, Init error\n");
				return NULL;
			}

			new->data = NULL;
			new->next = NULL;
			/* get tail data and put null node */
			lf->head = new;
			lf->tail = new;
			result = node->data;
			free(node);
		}
	}

	return result;
}
