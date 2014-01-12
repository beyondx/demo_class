#include <stdio.h>
#include <unistd.h>

ssize_t  Read(int fd, void *buf,  size_t count)
{
	ssize_t n ;
	while ((n = read(fd, buf, count)) < 0) {
		if (errno == EINTR) {
			continue;
		} else {
			perror("read");
			break;
		}
	}

	return n;
}
