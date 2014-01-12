#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main()
{
	int  fd = open("1.txt", O_WRONLY | O_CREAT | O_EXCL, 0666);
	
	if (fd < 0) {
		perror("fd");
		goto err;
	}

	fprintf(stderr, "1.This is from stderr\n");

	dup2(fd,  STDERR_FILENO);

	fprintf(stderr, "2.This is from stderr\n");


	close(fd);
	return 0;
err:
	return -1;
}


