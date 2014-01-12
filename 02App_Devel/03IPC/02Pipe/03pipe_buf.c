#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#define   PIPE_BUF	(4096 << 4)

/*
1. 导致管道阻塞的原因：
	a. 收发端没有同时存在
	b. 管道缓冲区已满
*/



int main()
{
	int fds[2] = {0};
	
	pipe(fds);

	char buf[PIPE_BUF] = {0};
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
	
	if (pid == 0) {
		printf("pid: %d child\n", getpid());
		memset(buf, 0, sizeof(buf));
		close(fds[1]);
		size_t n;
		while ((n = read(fds[0], buf, sizeof(buf)))!= 0) {
			printf("read: %d byte\n", n);
		}
		close(fds[0]);
		exit(0);
	} else {
		printf("pid: %d parent\n", getpid());
		close(fds[0]);
		//char *wbuf = (char *)calloc(1024, 1);
		char *wbuf = (char *)calloc(PIPE_BUF, 4);
		ssize_t n = 0;
		n = write(fds[1], wbuf, PIPE_BUF << 2);
		printf("write %d byte\n", n);
		
		close(fds[1]);
 		free(wbuf);
	}
	
	wait(NULL);

	return 0;
err:
	return -1;
}
