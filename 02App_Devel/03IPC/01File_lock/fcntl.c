#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
// atomic 原子性

#define BUFF_SIZE	1024

int foo1(void)
{
	printf("this is fcntl demo\n");
	int fd = open("1.txt", O_WRONLY |O_CREAT, 0666);

	if (fd < 0) {
		perror("open");
		goto err;
	}

	long flag = 0;
	if ((flag = fcntl(fd, F_GETFL)) < 0) {
		perror("fcnt;");
		goto err;
	}
	printf("flag = %x\n", flag);

	flag |= O_APPEND;
	printf("O_APPEND flag = %x\n", flag);
	
	if (fcntl(fd, F_SETFL, flag) < 0) {
		perror("fcntl");
		goto err;
	}
	
	dup2(fd, STDOUT_FILENO);	
	int i;
	for (i = 0; i < 10; ++i) {
		system("date");
		sleep(1);
	}

	close(fd);

	return 0;
err:
	return -1;
}

int  lock_status(int fd)
{
	struct flock st_flock;

	if (fcntl(fd, F_GETLK, &st_flock) < 0) {
		perror("fcntl");
		goto err;
	}

	switch (st_flock.l_type) {
		case F_UNLCK:
			printf("it's unlock\n");
			break;
		case F_RDLCK:
			printf("it's read lock by %d\n", st_flock.l_pid);
			break;
		case F_WRLCK:
			printf("it's write lock by %d\n", st_flock.l_pid);
			break;
	}
	
	return 0;
err:
	return -1;
}

int  lock_file(int fd, int type)
{
	struct flock st_flock = {0};

	st_flock.l_type = type;
	st_flock.l_whence = SEEK_SET;
		
	if (fcntl(fd, F_SETLKW, &st_flock) < 0) {
		perror("fcntl");
		goto err;
	}

	return 0;
err:
	return -1;

}

int lock_demo()
{
	int fd = open("1.txt", O_RDWR | O_CREAT, 0666);
	
	if (fd < 0) {
		perror("open");
		goto err;
	}
	
	lock_file(fd, F_RDLCK);
	printf("set write lock, %d\n", getpid());
	
	char buf[BUFF_SIZE];
	ssize_t n;
	sleep(5);
	#if 0
	while ((n = read(fd, buf, sizeof(buf))) != 0) {
		//write(STDOUT_FILENO, buf, n);
		printf("n = %d\n", n);
	}
	#endif
	printf("write lock release, %d\n", getpid());
	lock_file(fd, F_UNLCK);

	close(fd);

err:
	return -1;
}

int main()
{
	lock_demo();
	return 0;
}
