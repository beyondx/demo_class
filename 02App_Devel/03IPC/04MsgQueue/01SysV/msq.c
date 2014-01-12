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

	return 0;
}
