#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	int fds[2] = {0};
	
	pipe(fds);

	char buf[100] = {0};
#if 0
	write(fds[1], "hello", 10);
	read(fds[0], buf, sizeof(buf));
	printf("read: %s\n", buf);
#endif
	pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		goto err;
	}
	/*读端打开的前提下，写端才能完成，否则会租塞到打开为止，但如果明确关闭读端，直接产生SIGPIPE信号，导致进程异常退出*/	
	if (pid == 0) {
		printf("pid: %d child\n", getpid());
		memset(buf, 0, sizeof(buf));
		close(fds[0]);
		close(fds[1]);
	#if 0
		read(fds[0], buf, sizeof(buf));
		printf("buf:%s\n", buf);
		close(fds[0]);
	#endif
		exit(0);
	} else {
		printf("pid: %d parent\n", getpid());
		close(fds[0]);
		write(fds[1], "hello,world", 15);
		close(fds[1]);
	}
	
	wait(NULL);
 
	return 0;
err:
	return -1;
}
