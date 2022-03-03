#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>

/*
*�̳߳����������к͵ȴ���������һ��thread_worker_s
*�������������������������һ������ṹ
*/
typedef struct thread_worker
{
	/*�ص���������������ʱ����ô˺�����ע��Ҳ��������������ʽ*/
	void *(*process) (void *arg);
	void *arg;	/*�ص������Ĳ���*/
	struct thread_worker *next;
}thread_worker_s;

/*�̳߳ؽṹ*/
typedef struct
{
	pthread_mutex_t queue_lock;
	pthread_cond_t queue_ready;

	/*����ṹ���̳߳������еȴ�����*/
	thread_worker_s *queue_head;	

	/*�Ƿ������̳߳�*/
	int shutdown;
	pthread_t *threadid;

	/*�̳߳�������Ļ�߳���Ŀ*/
	int max_thread_num;

	/*��ǰ�ȴ����е�������Ŀ*/
	int cur_queue_size;
}thread_pool;

void *thread_routine (void *arg);

static thread_pool *pool = NULL;

void pool_init (int max_thread_num)
{
	pool = (thread_pool *) malloc (sizeof (thread_pool));

	pthread_mutex_init (&(pool->queue_lock), NULL);
	pthread_cond_init (&(pool->queue_ready), NULL);

	pool->queue_head = NULL;

	pool->max_thread_num = max_thread_num;
	pool->cur_queue_size = 0;

	pool->shutdown = 0;

	pool->threadid = (pthread_t *) malloc (max_thread_num * sizeof (pthread_t));
	int i = 0;
	for (i = 0; i < max_thread_num; i++)
	{
		pthread_create (&(pool->threadid[i]), NULL, thread_routine,
		 NULL);
	}
}


/*���̳߳��м�������*/
int pool_add_worker (void *(*process) (void *arg), void *arg)
{
	/*����һ��������*/
	thread_worker_s *newworker = (thread_worker_s *) malloc (sizeof (thread_worker_s));
	newworker->process = process;
	newworker->arg = arg;
	newworker->next = NULL;/*�����ÿ�*/

	pthread_mutex_lock (&(pool->queue_lock));

	/*��������뵽�ȴ�������*/
	thread_worker_s *member = pool->queue_head;
	if (member != NULL)
	{
		while (member->next != NULL)
		member = member->next;
		member->next = newworker;
	}
	else
	{
		pool->queue_head = newworker;
	}

	assert (pool->queue_head != NULL);

	pool->cur_queue_size++;
	pthread_mutex_unlock (&(pool->queue_lock));

	/*���ˣ��ȴ��������������ˣ�����һ���ȴ��̣߳�
	ע����������̶߳���æµ�����û���κ�����*/
	pthread_cond_signal (&(pool->queue_ready));
	return 0;
}

/*�����̳߳أ��ȴ������е����񲻻��ٱ�ִ�У������������е��̻߳�һֱ
    ����������������˳�*/
int pool_destroy ()
{
	if (pool->shutdown){
		return -1;/*��ֹ���ε���*/
	}
	pool->shutdown = 1;

	/*�������еȴ��̣߳��̳߳�Ҫ������*/
	pthread_cond_broadcast (&(pool->queue_ready));

	/*�����ȴ��߳��˳�������ͳɽ�ʬ��*/
	int i;
	for (i = 0; i < pool->max_thread_num; i++){
		pthread_join (pool->threadid[i], NULL);
	}
	free (pool->threadid);

	/*���ٵȴ�����*/
	thread_worker_s *head = NULL;
	while (pool->queue_head != NULL)
	{
		head = pool->queue_head;
		pool->queue_head = pool->queue_head->next;
		free (head);
	}
	/*���������ͻ�����Ҳ����������*/
	pthread_mutex_destroy(&(pool->queue_lock));
	pthread_cond_destroy(&(pool->queue_ready));

	free (pool);
	pool=NULL;
	return 0;
}

void *thread_routine(void *arg)
{
	while (1)
	{
		pthread_mutex_lock (&(pool->queue_lock));
		/*����ȴ�����Ϊ0���Ҳ������̳߳أ���������״̬; ע��
		pthread_cond_wait��һ��ԭ�Ӳ������ȴ�ǰ����������Ѻ�����*/
		while (pool->cur_queue_size == 0 && !pool->shutdown)
		{
			 pthread_cond_wait (&(pool->queue_ready), &(pool->queue_lock));
		}

		/*�̳߳�Ҫ������*/
		if (pool->shutdown)
		{
			/*����break,continue,return����ת��䣬ǧ��Ҫ�����Ƚ���*/
			 pthread_mutex_unlock (&(pool->queue_lock));
			 //printf ("thread 0x%x will exit\n", pthread_self ());
			 pthread_exit (NULL);
		}
		//printf ("thread 0x%x is starting to work\n", pthread_self ());

		/*assert�ǵ��Եĺð���*/
		assert (pool->cur_queue_size != 0);
		assert (pool->queue_head != NULL);

		/*�ȴ����г��ȼ�ȥ1����ȡ�������е�ͷԪ��*/
		pool->cur_queue_size--;
		thread_worker_s *thread_worker = pool->queue_head;
		pool->queue_head = thread_worker->next;
		pthread_mutex_unlock (&(pool->queue_lock));

		/*���ûص�������ִ������*/
		(*(thread_worker->process)) (thread_worker->arg);
		
		free (thread_worker);
		thread_worker = NULL;
	}
	/*��һ��Ӧ���ǲ��ɴ��*/
	pthread_exit (NULL);    
}

#if 1
//�����ǲ��Դ���

void *myprocess (void *arg)
{
	int len;
	char *buf = malloc(10);
	char tmp[128];
	memset(buf, 0, 10);
	strcpy(buf, "hello");
	len = strlen(buf);

	//sleep(3);
	printf ("threadid is 0x%x inputlen:%d input:%s\n", pthread_self(), len, buf);
	input_queue(len, buf);

	//printf ("threadid is 0x%x, working on task %d\n", pthread_self(),*(int *) arg);
	//sleep (1);/*��Ϣһ�룬�ӳ������ִ��ʱ��*/
	//printf ("threadid is 0x%x, task %d is finished\n", pthread_self(),*(int *) arg);
	return NULL;
}

void *myprocess2 (void *arg)
{
	int len;
	char *tmp = NULL;
	
	//sleep(3);
	tmp = (void*)output_queue(&len);
	printf ("threadid is 0x%x output:%s\n", pthread_self (), tmp);
	return NULL;
}

int main (int argc, char **argv)
{
	pool_init(10);/*�̳߳������������߳�*/
	init_queue();
	printf ("thread init ok \n");
	
    pool_add_worker (myprocess, NULL);
    pool_add_worker (myprocess2, NULL);

	/*�ȴ������������*/
	sleep (10);
	/*�����̳߳�*/
	pool_destroy();
	return 0;
}
#endif

