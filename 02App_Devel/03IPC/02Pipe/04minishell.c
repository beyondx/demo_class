#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* ls -la | wc -l*/
int main(int arg,char *argv[])
{
	int fds[2] = {0};
	
	pipe(fds);

	pid_t pid = fork();
	
	if (pid < 0) {
		perror("fork");
		exit(-1);
	} else if (pid == 0) {
		close(fds[0]);
		dup2(fds[1], STDOUT_FILENO);
		//system("ls -la");
		execlp("ls", "ls", "-la", NULL);
		exit(-1);
	} else {
		close(fds[1]);
		dup2(fds[0], STDIN_FILENO);
		system("wc -l");
	} 

	wait(NULL);
	
	return 0;
}
