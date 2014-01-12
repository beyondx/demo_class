#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>

#define   SEM_NAME	"/uplooking"

// SEM_FAILED   (sem_t *)-1
int main()
{
	sem_t *psem = sem_open(SEM_NAME, O_CREAT, 0666, 0);

	if (psem == SEM_FAILED) {
		perror("sem_open");
		goto err;
	}

	printf("pid %d sem_post\n", getpid());
	sem_post(psem);
	
	
	return 0;

err:
	return -1;
}
