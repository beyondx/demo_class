#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


//int new_signo = SIGRTMAX-4;

void sig_handler(int signo)
{
	switch (signo) {
		case SIGINT:
			printf("signo= %d, ctl + c\n", signo);
			break;
			break;
		default:
			printf("signo= %d, SIGRTMAX - 4\n", signo);
			break;
	}
	/*1-31中的信号因为没有队列技术，所以当收到连续相同的信号后，会丢弃后面的信号*/
	sleep(5);
}


int main(int argc, char *argv[])
{

	if (signal(SIGINT, sig_handler) == SIG_ERR) {
		perror("signal");
		exit(-1);
	}
	
        if (signal(SIGRTMAX - 4, sig_handler) == SIG_ERR) {
                perror("signal");
                exit(-1);
        }
	while (1) {
		printf("prg_name: %s, pid = %d\n", argv[0], getpid());
		/*sleep 不会阻塞信号的捕获和处理*/
		sleep(3);
	}

	return 0;
}
