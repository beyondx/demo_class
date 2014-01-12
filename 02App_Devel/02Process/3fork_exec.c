#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define  CHILD_NUM	3

int main()
{
	printf("this is fork+exec demo\n");
	
	int count = 0;
	for (; count < CHILD_NUM; ++count) {
		sleep(1);
		int pid = fork();
		switch (pid) {
			case -1:
				perror("fork");
				goto err;
				break;
			case 0:
				//child
				printf("child: pid(%d), ppid(%d)\n", getpid(), getppid());
				execlp("sh", "sh", "-c", "date", NULL);
				break;
			default:
				//parent;
				break;
		}
	}
	
	for (count = 0; count < CHILD_NUM; ++count) {
		waitpid(0, NULL, 0);
	}	

	printf("all child done\n");
	return 0;

err:
	return -1;
}
