#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main()
{
	char rbuf = 0;

	int fd = open("/etc/passwd", O_RDONLY);

	int fd_null = open("/dev/null", O_RDWR);

	if (fd == -1) {
		perror("open");
		goto err;
	}

	while (read(fd, &rbuf, 1) != 0) {
		write(fd_null, &rbuf, 1);
	}

	close(fd);
	close(fd_null);
	return 0;
	
err:
	return -1;
}
