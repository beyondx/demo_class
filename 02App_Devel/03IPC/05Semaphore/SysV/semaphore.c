#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#define  IPC_NAME	"/tmp"
#define  PROJ_ID	'x'

#define   errExit(str) \
	do { perror(str);  exit(-1); }while(0)
#define   errReturn(str, ret) \
	do { perror(str);  return ret; }while(0)


union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
#if defined (__LINUX__) || defined (__linux__)
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
				    (Linux specific) */
#endif
};
int  up_sem_init(int nsems, int init_val)
{
	key_t key = ftok(IPC_NAME, PROJ_ID);
	
	if (key == -1) {
		errReturn("ftok", key);
	}	

	//int semid = semget(key, 1, IPC_CREAT |0666);
	int semid = semget(key, 1, 0);
	
	if (semid < 0) {
		errReturn("semget", semid);
	}

	union semun  semun;
	semun.val = init_val;

	if (semctl(semid, 0, SETVAL, semun) < 0) {
		errReturn("semctl", -1);
	}	

	return semid;
}

// P:申请资源 (wait)
int  up_sem_p(int semid, int sem_num)
{
	struct sembuf buf = {
			.sem_num = sem_num, //信号集的下标
			.sem_op  = -1,
			.sem_flg = SEM_UNDO, 
		};
	while (semop(semid, &buf, 1) < 0) {
		if (errno == EINTR) {
			continue;
		} else {
			errReturn("semop", -1);
		}
	}

	return 0;
}


// V:释放资源 (post)
int  up_sem_v(int semid, int sem_num)
{
	struct sembuf buf = {
			.sem_num = sem_num, //信号集的下标
			.sem_op  = 1,
			.sem_flg = SEM_UNDO, 
		};
	while (semop(semid, &buf, 1) < 0) {
		if (errno == EINTR) {
			continue;
		} else {
			errReturn("semop", -1);
		}
	}
	return 0;
}

int up_sem_destroy(int semid)
{
	return semctl(semid, 0, IPC_RMID);
}

int main()
{
	printf("process is running (pid = %d)\n", getpid());
	
	int semid = up_sem_init(1, 1);

	printf("up_sem_init doen\n");

	if (up_sem_p(semid, 0) < 0) {
		errReturn("up_sem_p", -1);
	}
	printf("i will block 15 seconds, semid: %d\n", semid);
	sleep(15);
	
	if (up_sem_v(semid, 0) < 0) {
		errReturn("up_sem_v", -1);
	}
	printf("unblock sem, semid = %d\n", semid);
#if 0	
	if (up_sem_destroy(semid) < 0) {
		errReturn("up_sem_destroy", -1);
	}	
	printf("sem del\n");
#endif
	return 0;
}

