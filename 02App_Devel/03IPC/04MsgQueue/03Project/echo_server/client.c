#include <stdio.h>
#include "utils.h"

int msg_handler(int msq_id, const struct msq_buf *buf)
{
	struct msq_buf rbuf = {0};

#ifdef DEBUG
	printf("client send to server: %s\n", buf->mdata);
#endif	
	if (msgsnd(msq_id, buf, strlen(buf->mdata) + 1, 0) < 0) {
		errReturn ("msqsnd", -1);
	}
	printf("wait for response from server\n");
	
	if (msgrcv(msq_id, &rbuf, BUFF_SIZE, buf->mtype, MSG_NOERROR) < 0) {
		errReturn ("msqsnd", -1);
	}
	printf("receive from server: %s\n", rbuf.mdata);

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

	msq_status(msq_id);

	int n, i;
	struct  msq_buf  wbuf = {0};
	wbuf.mtype = 20 + getpid() % 10;	

	while (1) {
		if (fgets(wbuf.mdata, BUFF_SIZE - 1, stdin) != NULL) {
			if (strncmp(wbuf.mdata, "exit", 4) == 0) {
				printf("byebye\n");
				goto out;
			}
			msg_handler(msq_id, &wbuf);		

		} else {
			errExit("fgets");
		}
		
	}

	
	#if 0

	for (i = 0; i < 1024; ++i) {
		printf("i = %d\n", i);
		for (n = 0; n < sizeof s/ sizeof s[0]; ++n) {
			msgsnd(msq_id, s + n, strlen(s[n].mdata) + 1, 0);
			//msgsnd(msq_id, s + n, BUF_SIZE + 1, 0);
			msq_status(msq_id);
		}
	}
	#endif

	printf("msqsnd over\n");

out:
	return 0;
}
