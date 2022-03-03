#include "queue.h"
#define log_printf printf

struct queue_info g_queue_head;

/*
* Initializing queue
*/
void init_queue()
{
	memset(&g_queue_head, 0, sizeof(struct queue_info));
	g_queue_head.head = NULL;
	g_queue_head.tail = NULL;
	g_queue_head.queue_cache = malloc(QUEUE_CACHE_SIZE * sizeof(queue_list));
	g_queue_head.queue_cache_state = 0;
	g_queue_head.queue_cache_num = 0;
	sem_init(&g_queue_head.recv_data_sem, 0, 0);
	pthread_mutex_init(&g_queue_head.queue_lock, NULL);
	return ;
}

/*
* input queue cache
*/
int input_queue(int data_len, unsigned char *data)
{
	int i = 0, exists = 0;
	struct queue_list *queue_node = NULL;

	pthread_mutex_lock(&g_queue_head.queue_lock);
	for(; i < QUEUE_CACHE_NUM; i++) {
		if(!(BIT(i) & g_queue_head.queue_cache_state)) {
			exists = 1;
			break;
		}
	}
	if(!exists) {
		log_printf("Warning: queue cache is full\n");
		g_queue_head.queue_cache_num = QUEUE_CACHE_MAX_NUM;
		pthread_mutex_unlock(&g_queue_head.queue_lock);
		return -1;
	}

	g_queue_head.queue_cache_state |= BIT(i);
	pthread_mutex_unlock(&g_queue_head.queue_lock);

	queue_node = g_queue_head.queue_cache + (sizeof(struct queue_list) * i);
	queue_node->data_len = data_len;
	queue_node->data = data;
	sem_post(&g_queue_head.recv_data_sem);
	return 0;
}

/*
* input queue
*/
int input_queue(int data_len, unsigned char *data)
{
	struct queue_list *queue_node = NULL;

	if(data_len <= 0) {
		log_printf("error input data len is :%d\n", data_len);
		return -1;
	}

	/* input queue cache frist */
	if(g_queue_head.queue_cache_num < QUEUE_CACHE_MAX_NUM) {
		return input_queue_cache(data_len, data);
	}

	queue_node = (struct queue_list *)malloc(sizeof(struct queue_list));
	if(queue_node == NULL) {
		log_printf("malloc queue error!\n");
		return -1;
	}

	queue_node->data_len = data_len;
	queue_node->data = data;

	pthread_mutex_lock(&g_queue_head.queue_lock);
	if(g_queue_head.head == NULL) {
		/* 队列为空 */
		g_queue_head.head = queue_node;
		queue_node->next = NULL;
		g_queue_head.tail = queue_node;
	} else {
		g_queue_head.tail->next = queue_node;
		g_queue_head.tail = queue_node;
	}
	pthread_mutex_unlock(&g_queue_head.queue_lock);

	sem_post(&g_queue_head.recv_data_sem);
	return 0;
}

/*
* output queue
*/
void *output_queue(int *data_len)
{
	struct queue_list *queue_node = NULL;

	sem_wait(&g_queue_head.recv_data_sem);

	pthread_mutex_lock(&g_queue_head.queue_lock);
	if(g_queue_head.head == NULL) {
		pthread_mutex_unlock(&g_queue_head.queue_lock);
		log_printf("queue is null!");
		return NULL;
	} else {
		queue_node = g_queue_head.head;
		*data_len = queue_node->data_len;
		g_queue_head.head = queue_node->next;

		pthread_mutex_unlock(&g_queue_head.queue_lock);
		free(queue_node);
		return queue_node->data;
	}
	return NULL;
}

/*
int main ()
{
	int len;
	char buf[128];
	char tmp[128];
	init_queue();

	memset(buf, 0, sizeof(buf));
	strcpy(buf, "hello");

	input_queue(strlen(buf), buf);

	memset(tmp, 0, sizeof(tmp));
	output_queue(&len, tmp);
	printf("buf:%s\n", tmp);
} */

