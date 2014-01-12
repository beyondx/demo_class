#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main()
{
	fprintf(stderr, "this is from stderr\n");

	//dup(2);
	int fd = dup(STDERR_FILENO);
	
	if (fd < 0) {
		perror("dup");
		goto err;
	}

	char buf[100] = {0};
	snprintf(buf, sizeof(buf) - 1, "this is dup file descriptor: %d\n", fd);
	write(fd, buf, strlen(buf) + 1);

	close(fd);

	return 0;
err:
	return -1;
}
