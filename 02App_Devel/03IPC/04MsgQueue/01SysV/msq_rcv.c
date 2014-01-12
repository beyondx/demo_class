#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define  IPC_PATH	"/tmp"
#define  IPC_PROJ_ID	'x'

#ifndef   errExit
#include <stdlib.h>
#include <errno.h>
#define   errExit(str)	\
		do { perror(str); \
		 exit(-1); }while(0)
#endif

#ifndef   errReturn
#include <stdlib.h>
#include <errno.h>
#define   errReturn(str, retno)	\
		do { perror(str); \
		     return retno; }while(0)
#endif



#define  BUF_SIZE       1024
struct  msq_buf {
        long mtype;
        char mdata[BUF_SIZE];
};

int msq_status(int msq_id)
{
	struct msqid_ds  buf = {0};
	
	if (msgctl(msq_id, IPC_STAT, &buf) < 0) 
		errReturn("msgctl", -1);

	printf("--------status---------\n");
	printf("stime: %s", ctime(&buf.msg_stime));
	printf("rtime: %s", ctime(&buf.msg_rtime));
	printf("ctime: %s", ctime(&buf.msg_ctime));
	printf("msg_qnum: %lu\n", buf.msg_qnum);
	printf("msg_qbytes: %lu\n", buf.msg_qbytes);
	printf("lspid: %hu\n", buf.msg_lspid);
	printf("lrpid: %hu\n", buf.msg_lrpid);
	printf("--------status---over-------\n");
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

	msq_status(msq_id);
	printf("msg_rev start\n");
	struct msq_buf buf = {0};
	ssize_t rev_num = 0;
	while (1) {
		if ((rev_num = msgrcv(msq_id, &buf, BUF_SIZE - 1, 0, MSG_NOERROR)) > 0) {
			printf("rev_num: %ld\n", rev_num);
			printf("type: %d, data: %s\n", buf.mtype, buf.mdata);
		}else {
			perror("msgrcv");
			break;
		}
	}

	msq_status(msq_id);


	return 0;
}
