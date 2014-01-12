#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#define    errExit(str)	\
	do { perror(str); exit(-1); }while (0)

char *curTime()
{
	time_t t;
	t = time(NULL);
	return  ctime(&t);
}

int main()
{
	int fd = open("1.txt", O_RDWR| O_CREAT, 0666);

	if (fd == -1) {
		errExit("open");
	}
	
	int flag = fcntl(fd, F_GETFL);
	
	flag |= O_APPEND;
	
	if (fcntl(fd, F_SETFL, flag) < 0) {
		errExit("fcntl");
	}

	//关闭标准输出的默认缓冲功能
	/*
	1. 缓冲区满
	2. '\n',强行刷新缓冲区
	3. fflush, 手动刷新缓冲区
	4. 关闭缓冲区
	*/
	setbuf(stdout, NULL);
	dup2(fd, STDOUT_FILENO);
	
	int n = 10;
	while (--n) {
		sleep(1);
		printf("pid:%d:%s", getpid(), curTime());
	}	
	
	close(fd);

	return 0;
}
