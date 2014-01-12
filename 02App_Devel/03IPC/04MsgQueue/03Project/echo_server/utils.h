#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <string.h>

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

#define  BUF_SIZE  4096
#define  BUFF_SIZE BUF_SIZE

struct  msq_buf {
        long mtype;
        char mdata[BUF_SIZE];
};

static inline int msq_status(int msq_id)
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
