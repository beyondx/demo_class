#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define   MY_LOG	"/tmp/daemon_%d.log"

void Daemon(int nochdir, int noclosefd)
{
#ifdef DEBUG
	printf("getpgid:%d, pid:%d\n", getpgid(0), getpid());
#endif
	//1. ppid = 1
	int pid = fork();
	
	switch (pid) {
		case -1:
			perror("fork");
			exit(-1);
			break;
		case 0:	
			usleep(1000);
			break;
		default:
			exit(0);
			break;
	}
#if 0
	pid = fork();
	if (pid < 0 || pid > 0) {
		exit(0);
	}
#endif
	//2. primer of group
	if (setsid() < 0) {
		perror("setsid");
		goto err;
	}
#ifdef DEBUG
	printf("getpgid:%d, pid:%d, ppid:%d\n", getpgid(0), getpid(), getppid());
#endif
	// umask(0)
	umask(0);
	
	//cwd
	if (!nochdir) {
		chdir("/");
	}
	
	//close 1, 2, 3, > /dev/null
	int fd = open("/dev/null", O_RDWR);
	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	printf("never see me\n");	
	return 0;

err:
	exit(-1); 
}

int main()
{
	char log_file_name[255] = {0};

	snprintf(log_file_name, sizeof(log_file_name), MY_LOG, getpid());

	FILE *pf_log = fopen(log_file_name, "a+");
	Daemon(0, 0);
	int i = 10;
	while (--i) {
		fprintf(pf_log, "program(%d) is running\n", getpid());
		fflush(pf_log);
		sleep(2);
	}
	fclose(pf_log);
	return 0;
}

