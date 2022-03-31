#ifndef LFQUEUE_H
#define LFQUEUE_H

/* lock-free queue node */
struct lock_free_queue_node
{
	void *data;
	//volatile struct lock_free_queue_node *next;
	struct lock_free_queue_node *next;
};

struct lf_opt {
    struct lock_free_queue_node *head;
    struct lock_free_queue_node *tail;
};

struct lf_opt *lf_queue_init();

void lf_queue_deinit(struct lf_opt *lf);

int lf_queue_push(struct lf_opt *lf, void *pdata);

void *lf_queue_pop(struct lf_opt *lf);
#endif /*  LFQUEUE_H */
