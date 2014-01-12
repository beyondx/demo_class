#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

#define  SEM_PATH_NAME	"/tmp"
#define  SEM_PROJ_ID	'x'

#define  errExit(str)	\
	do { perror(str); exit(-1); } while(0)


#define  errReturn(str, n)	\
	do { perror(str); return (n); } while(0)



union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
#if defined (__LINUX) || defined (__linux__)
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
				    (Linux specific) */
#endif
};

int sem_p(int semid)
{
	struct sembuf buf = {
			.sem_num = 0,
			.sem_op = -1,
			.sem_flg = SEM_UNDO, 
			};

	return semop(semid, &buf, 1);
}

int sem_v(int semid)
{
	struct sembuf buf = {
                        .sem_num = 0,
                        .sem_op = 1,
                        .sem_flg = SEM_UNDO,
                        };

        return semop(semid, &buf, 1);
}


int main()
{
	key_t key = ftok(SEM_PATH_NAME, SEM_PROJ_ID);
	
	if (key == -1) {
		errExit("ftok");
	}	
	//1. 创建信号量
	int semid = semget(key, 1, IPC_CREAT | 0666);

	if (semid == -1) {
		if (errno == EEXIST) {
			//TODO
			perror("semget");
		} else {
			errExit("semget");
		} 
	}
	union semun arg;
	arg.val = 0;
	//2. 初始化信号量
	if (semctl(semid, 0, SETVAL, arg) < 0) {
		errExit("semctl");
	}
	

	//3. 操作信号量
	//P: 申请资源, 信号量非0, - 1; 信号量量为0阻塞

	//V: 释放资源

	pid_t pid = fork();

	switch (pid) {
		case 0:
			//child
			printf("child, pid = %d, ppid = %d\n", getpid(), getppid());
			sem_p(semid);
			printf("child block sem\n");
			sleep(3);
			sem_v(semid);
			printf("child relase sem\n");
			exit(0);
			break;
		case -1:
			errExit("fork");
			break;
		default:
			//parent		
			break;
	}

	printf("parent, pid = %d, child_id = %d\n", getpid(), pid);
	printf("parent working\n");
	sleep(5);
	sem_v(semid);
	printf("parent realse sem\n");

	if (wait(NULL) < 0) {
		errExit("wait");
	}

	//4. 删除信号量
	if (semctl(semid, 0, IPC_RMID) < 0) {
		errExit("semctl");
	}

	return 0;
}

