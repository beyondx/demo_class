#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define  errExit(str)	\
	do { perror(str); exit(-1); } while(0)


void *thr_demo(void *args)
{
	printf("this is thr_demo thread, pid: %d, tid: %lu\n", getpid(), pthread_self());

	printf("args: %d\n", *(int *)args);

	return (void *)10;
}


int main()
{
	pthread_t pt1;
	
	int arg = 100;
	int ret;
	
	ret = pthread_create(&pt1, NULL, thr_demo, &arg);

	if (ret != 0) {
		errExit("pthread_create");
	}
	
	printf("this is main thread, pid = %d, tid = %lu\n", getpid(), pthread_self());	
  	ret = pthread_join(pt1, &arg);

	if (ret != 0) {
		errExit("pthread_join");
	}	
	printf("arg = %d\n", arg);
	return 0;
}

