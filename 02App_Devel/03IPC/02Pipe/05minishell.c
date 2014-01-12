#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* ls -la | wc -l*/
int do_cmd(const char *cmd)
{
	int fds[2] = {0};
	
	pipe(fds);

	pid_t pid = fork();
	
	if (pid < 0) {
		perror("fork");
		exit(-1);
	} else if (pid == 0) {
		//execlp("sh", "sh", "-c", cmd,  NULL);
		execlp(cmd,  NULL);
		exit(-1);
	} else {
	} 

	wait(NULL);
	
	return 0;
}
#define BUF_SIZE	255

int cmd_handler(char *line)
{
	size_t n = strlen(line);
	line[n - 1] = '\0';
	
	if (strncmp(line, "cd", 2) == 0) {
		if (chdir(line + 3) < 0) {
			perror("chdir");
			goto err;
		}
	} else {
		do_cmd(line);	
	}

	return 0;
err:
	return -1;
}

int   shutdown = 0;

int main(int argc, char *argv[])
{
	printf("\tMiniShell  V1.0\n");
	
	char rbuf[BUF_SIZE] = {0};
	

	while (! shutdown) {
		printf("> ");
		fflush(stdout);
		if  (fgets(rbuf, BUF_SIZE - 1, stdin)) {
			if (strncasecmp(rbuf, "exit", 4) == 0) {
				shutdown = 1;
			} else {
				cmd_handler(rbuf);
			}	
		} else {
			shutdown = 1;
		}
	}
	
	return 0;
}

