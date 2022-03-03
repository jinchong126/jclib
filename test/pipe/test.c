#include <sys/wait.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

#define WRITE_SIZE 1024
#define WRITE_COUNT 50

void *eloop_array_run(void *arg)
{
	int ret, count = 0;
	int fd;
    int status = 0;
	char buf[WRITE_SIZE];

	memcpy(&fd, arg, sizeof(int));
        printf("pthread fd1 %d\n", fd);
 
	while(1) {
		printf("begin write size %d\n", sizeof(buf));
		ret = write(fd, buf, sizeof(buf));
		printf("write ret %d errno %d\n", ret, errno);
		count ++;
		if(count > WRITE_COUNT)
			break;
	}
}

void *eloop_array_run2(void *arg)
{
	int ret, count = 0;
	int fd;
    int status = 0;
	char buf[WRITE_SIZE];

	memcpy(&fd, arg, sizeof(int));
        printf("pthread2 fd1 %d\n", fd);
 
	while(1) {
		printf("begin2 write size %d\n", sizeof(buf));
		ret = write(fd, buf, sizeof(buf));
		printf("write2 ret %d errno %d\n", ret, errno);
		count ++;
		if(count > WRITE_COUNT)
			break;
	}
}

int main(int argc, char *argv[])
{
    int fd[2];
    pid_t pid;
    pthread_t tid;
    char read_buffer[1024*4] = {0};
    int read_count = 0;
     
    //创建管道
    if (pipe(fd) < 0)
    {
        printf("Create pipe failed.");
        return -1;
    }

	printf("fd0 %d fd1 %d\n", fd[0], fd[1]);
	int flags = fcntl(fd[1], F_GETFL, 0);
	fcntl(fd[1],  F_SETFL, flags | O_NONBLOCK);
	flags = fcntl(fd[0], F_GETFL, 0);
	fcntl(fd[0],  F_SETFL, flags | O_NONBLOCK);
 
    if(pthread_create(&tid, NULL, eloop_array_run, &(fd[1])) != 0) {
       printf("Can't create thread (maybe there are too many other threads)");
       return -1;
   }
    if(pthread_create(&tid, NULL, eloop_array_run2, &(fd[1])) != 0) {
       printf("Can't create thread (maybe there are too many other threads)");
       return -1;
   }


    printf("wait fd1 \n");
    while(1){
		sleep(1);
	    read_count = read(fd[0], read_buffer, sizeof(read_buffer));
		printf("Content under current directory: %d errno %d \n ", read_count, errno);
    }
}
