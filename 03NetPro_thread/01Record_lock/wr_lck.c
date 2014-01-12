#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#define    errExit(str) \
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
		perror("open 1.txt");
		if (errno == EEXIST) {
			fd = open("1.txt", O_RDWR | O_APPEND);
		} else {
			exit(-1);
		}
	}

	struct flock  lock = {0};
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 10;
	lock.l_len = 40;

	if (fcntl(fd, F_SETLKW, &lock) == -1) {
		errExit("fcntl");
	}
	printf("pid:%d set write lock file %s", getpid(), curTime());
	sleep(8);

	lock.l_type = F_UNLCK;
	if (fcntl(fd, F_SETLK, &lock) == -1) {
		errExit("fcntl");
	}
	printf("pid:%d set write unlock %s", getpid(), curTime());



	close(fd);
	return 0;
}
