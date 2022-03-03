#ifndef __QUEUE_H__
#define __QUEUE_H__
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

#define QUEUE_CACHE_SIZE 2048
#define QUEUE_CACHE_MAX_NUM 10

#define BIT(x) (1 << (x))

struct queue_info
{
	struct queue_list *head;
	struct queue_list *tail;
	sem_t recv_data_sem;
	void *queue_cache;
	int queue_cache_state;	/* 位表示 */
	int queue_cache_full;	/* 0 not full, 1 full */
	pthread_mutex_t queue_lock;
};

struct queue_list
{
	struct queue_list *next;
	int data_len;
	unsigned char *data;
};

#endif /* __QUEUE_H__ */
