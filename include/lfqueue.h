#ifndef LFQUEUE_H
#define LFQUEUE_H

/* lock-free queue node */
struct lock_free_queue_node
{
	void *data;
	//volatile struct lock_free_queue_node *next;
	struct lock_free_queue_node *next;
};

int lf_queue_init();

int lf_queue_push(void *pdata);

void *lf_queue_pop();
#endif /*  LFQUEUE_H */
