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
	
	if (pid == 0) {
		printf("pid: %d child\n", getpid());
		memset(buf, 0, sizeof(buf));
		close(fds[1]);
		read(fds[0], buf, sizeof(buf));
		printf("buf:%s\n", buf);
		close(fds[0]);
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
