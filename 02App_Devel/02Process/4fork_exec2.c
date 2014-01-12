#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define  CHILD_NUM	3
#define  SERVER_NUM	30

int main()
{
	printf("this is fork+exec demo\n");
	
	int count = 0;
	int child_dead_num;
	
	for (child_dead_num = 0; child_dead_num < SERVER_NUM; ) {
		for (; count < CHILD_NUM; ++count) {
			usleep(1500000);
			int pid = fork();
			switch (pid) {
				case -1:
					perror("fork");
					goto err;
					break;
				case 0:
					//child
					printf("child: pid(%d), ppid(%d)\n", getpid(), getppid());
					sleep(3);
					execlp("sh", "sh", "-c", "date", NULL);
					break;
				default:
					//parent;
					//printf("creat_new_child:%d\n", pid);
					break;
			}
		}
		if (waitpid(0, NULL, 0) < 0) {
			perror("waitpid");
		} else {
			++child_dead_num;
			--count;
			printf("child_dea_num:%d, count:%d\n", child_dead_num, count);
		}
	}
	#if 1
	for (count = 0; count < CHILD_NUM; ++count) {
		waitpid(0, NULL, 0);
	}
	#endif	

	printf("all child done\n");
	return 0;

err:
	return -1;
}
