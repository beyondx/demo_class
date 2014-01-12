#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int msg_handler(int msq_id, const struct msq_buf *buf)
{
	struct msq_buf  mybuf = {0};
	mybuf.mtype = buf->mtype;
	char *ptr = buf->mdata;
	
	int i;
	for (i = 0; *ptr != '\0'; ++i) {
		if ((*ptr > 'a' - 1) && (*ptr < 'z' + 1)) {
			mybuf.mdata[i] = *ptr - 32;
		} else {
			mybuf.mdata[i] = *ptr;
		}
		++ptr;
	}
	mybuf.mdata[i] = '\0';
#ifdef DEBUG
	printf("server send to %d: %s\n", mybuf.mtype, mybuf.mdata);
#endif
	if (msgsnd(msq_id, &mybuf, strlen(mybuf.mdata) + 1, 0) < 0) {
		errReturn ("msgsnd", -1);
	}

	return 0;
}


int main()
{
	key_t key = ftok(IPC_PATH, IPC_PROJ_ID);
	int msq_id;	


	if (key == -1) 
		errExit("ftok");
	else 
		printf("ftok ok\n");
	
	if ((msq_id = msgget(key, IPC_CREAT | IPC_EXCL| 0666)) < 0) {
		if (errno == EEXIST) {
			msq_id = msgget(key, 0); 
		} else 
			errExit("msgget");
	}

	printf("server start, pid = %d\n", getpid());
	struct msq_buf buf = {0};
	ssize_t rev_num = 0;

	while (1) {
		if ((rev_num = msgrcv(msq_id, &buf, BUF_SIZE, 0, MSG_NOERROR)) > 0) {
			printf("server receive from %d: %s\n", buf.mtype, buf.mdata);
			msg_handler(msq_id, &buf);
		}else {
			perror("msgrcv");
			break;
		}
	}

	msq_status(msq_id);

	return 0;
}
