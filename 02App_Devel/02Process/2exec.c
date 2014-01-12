#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

extern char **environ;
void get_env()
{
	char **penv = environ;

	while (*penv != NULL) {
		printf("%s\n", *penv++);
	}

	printf("-----get_env done----\n");
	printf("shell:%s\n", getenv("SHELL"));
	printf("user:%s\n", getenv("USER"));
}

void execl_test(const char *cmd)
{
	execl("/bin/bash", "/bin/bash", "-c", cmd, NULL);
	fprintf(stderr,"execl failed\n");
}
void execlp_test()
{
	execlp("ls", "ls","-la", "/tmp", NULL);
}

void execvp_test()
{
	char *cmd[] = {
		"ls",
		"-l",
		"/",
		NULL,
	};

	execvp("ls", cmd);
}
int main(int argc, char *argv[])
{
	//get_env();
	//execl_test(argv[1]);
	execvp_test();
	printf("hello,world");

	return 0;
}

