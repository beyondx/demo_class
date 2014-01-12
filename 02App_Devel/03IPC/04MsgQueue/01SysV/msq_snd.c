#include <stdio.h>
#include <string.h>
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

#define  BUF_SIZE	1024
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

	struct  msq_buf s[] = {
		{1, "tom"}, {2, "jack"}, {3, "niko"}, {4, "richard"},
		{5, "kevin"}, {6, "mark"}, {7, "tube"}, {8, "cathcy"},
	};

	int n, i;
	for (i = 0; i < 1024; ++i) {
		printf("i = %d\n", i);
		for (n = 0; n < sizeof s/ sizeof s[0]; ++n) {
			msgsnd(msq_id, s + n, strlen(s[n].mdata) + 1, 0);
			//msgsnd(msq_id, s + n, BUF_SIZE + 1, 0);
			msq_status(msq_id);
		}
	}


	printf("msqsnd over\n");


	return 0;
}
