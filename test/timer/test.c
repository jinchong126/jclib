#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timerlib.h"
#include <signal.h> 
#include <unistd.h> 
#include <stdio.h> 

void timer_handler(void *arg)
{
	printf("timer is out\n");
	return ;
}

int main()
{
	int id;
	signal(SIGALRM, NULL); 

	/* init */
	timer_init();

	while(1){
	printf("timer is add\n");
	id = timer_add(1, 0, &timer_handler, NULL);
	printf("timer add end \n");

/*
	printf("timer rem id(%d)\n", id);
	timer_rem(id, NULL);
	printf("timer rem end sleep(5)\n");
*/
	sleep(5);
	}

	timer_destroy();
	printf("program is exist\n");
}
