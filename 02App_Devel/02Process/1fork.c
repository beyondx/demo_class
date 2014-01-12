#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* return : keyword, 返回数值， return 0 == exit(0)
   exit:  function， 导致进程退出

 僵尸进程：进程任务已经结束，对于的资源基本释放完成
	只是父进程没有替其从进程列表中清除，所以已僵尸状态出现
	如何避免：
	1） 父进程承担责任，替其收尸
	2） 父进程忽略，自主动消除， SIGCHLD
	3） 杀掉父进程， 让子进程变为孤儿进程，init替其收尸
 孤儿进程： 父进程抢着孤儿前过世，则子进程为孤儿进程 
*/

int foo()
{
	return 0;
	//exit(0);
}

int a = 10;


int main()
{
	printf("this first process ,pid = %d\n", getpid());

	int b = 20;
	int *c = (int *)malloc(sizeof(*c));
	*c = 30;
	
	int pid = fork();

	if (pid < 0) {
		perror("fork");
		goto err;
	}
	if (pid == 0) {
		//child
		printf("child:pid = %d, ppid = %d\n", getpid(), getppid());
		printf("child:a = %d, b = %d, *c = %d\n", a, b, *c);
		sleep(10);
		printf("child:pid = %d, ppid = %d\n", getpid(), getppid());
		printf("child:a = %d, b = %d, *c = %d\n", a, b, *c);
		exit(10);
	} else {
		//parent
		printf("parent,pid = %d, child pid = %d\n", getpid(), pid);
		printf("parent:a = %d, b = %d, *c = %d\n", a, b, *c);
		sleep(1);	
		printf("parent change:a = %d, b = %d, *c = %d\n", ++a, ++b, (*c += 1));
		exit(0);
	}
	printf("this line will show twice\n");
	int child_ret = 0;
	
	//wait 会导致父进程阻塞，等待子进程SIGCHLD信号
	if (wait(&child_ret) < 0) {
		perror("wait");
	}

	printf("wait unblock\n");
	if (WIFEXITED(child_ret)) {
		printf("child return = %d\n", WEXITSTATUS(child_ret));
	}
	free(c);

	return 0;
err:
	return -1;
}
