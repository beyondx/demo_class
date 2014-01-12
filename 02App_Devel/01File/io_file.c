#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#define  BUF_SIZE 	4096

int main(int argc, char **argv)
{
	int fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, 0666);

	if (fd == -1) {
		perror("open");
		if (errno == EEXIST) {
			fd = creat(argv[1], 0666);
		} else {
			return -1;
		}
	}

	char wbuf[BUF_SIZE] = {0};
	time_t t;
	int i;
	for (i = 0; i < 5; ++i) {
		time(&t);
		sleep(1);
		snprintf(wbuf, BUF_SIZE - 1, "%s", ctime(&t));
		write(fd, wbuf, strlen(wbuf) + 1);
	}

	close(fd);
	//unlink(argv[1]);

	return 0;
}
