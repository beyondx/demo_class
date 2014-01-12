#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

void stmode_handler(mode_t mode)
{
	switch (mode & S_IFMT){
		case S_IFREG:
			printf("regular file\n");
			break;
		case S_IFBLK:
			printf("block device file\n");
			break;
		case S_IFCHR:
			printf("char device file\n");
			break;
		case S_IFLNK:
			printf("symbolic link file\n");
			break;
		case S_IFSOCK:
			printf("socket file\n");
			break;
		default:
			printf("unkown file type\n");
			break;
	}
}

void buf_handler(const struct stat *buf)
{
	printf("file_size: %lld\n", buf->st_size);
	printf("inode:%llu\n", buf->st_ino);

	printf("st_mode:%x\n", buf->st_mode);
	
	stmode_handler(buf->st_mode);

}

int main(int argc, char **argv)
{
	assert(argc > 1);

	struct stat buf = {0};
	
	if (stat(argv[1], &buf) < 0) {
		perror("stat");
		goto err;
	}
	
	buf_handler(&buf);	

	return 0;
err:
	return -1;
}
