#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "chat.h"

#define  MY_TYPE  1

struct msg_buf *s_buf ;
int id;
int msg_id;
void menu(void)
{
	printf("\t\t %s\t%s %s\n", PRG_NAME, AUTHOR, VERSION);
	printf("--------------------------------\n");
	printf("\t+ (l)ogin\n");
	printf("\t+ (Q)uit\n");
	printf("\t+ (C)hat\n");
	printf("\t+ (B)roadCast\n");
	printf("\t+ (F)ile Transfer\n");
	printf("\t+ (L)ist of Users\n");
	printf("\t+ (H)elp\n");
	printf("--------------------------------\n");
}

void login(void)
{
	printf("Your Name:");
	fflush(stdout);
	char name[MAX_NAME]= {0};
	scanf("%s", name);
	int id = getpid() % 100;
	snprintf(s_buf->mtext, BUF_SIZE, "l:00:%d#%s", id, name);
}

void chat(int msg_id)
{
	printf("example: @id:message\n");
	char buf[1024] = {0};
	int dst_id;
	scanf("@%d:%s", &dst_id, buf);
	#ifdef DEBUG
	printf("@%d:%s", dst_id, buf);
	#endif
	s_buf->mtype = dst_id;
	snprintf(s_buf->mtext, BUF_SIZE, "C:%d:%s", dst_id, buf);
	
	msgsnd(msg_id, s_buf, strlen(s_buf->mtext) + 1, 0);

}
int main()
{
	msg_id = msgget((key_t)0x12345, IPC_CREAT|0600);

	if (msg_id < 0) {
		perror("msgget");
		goto err;
	}

	s_buf = (struct msg_buf *)malloc(sizeof(struct msg_buf));
	assert(s_buf != NULL);

	s_buf->mtype = MY_TYPE;
	int n;
	id = getpid() % 100;
	menu();
	char buf[BUF_SIZE] = {0};
	char c;
	while (1) {
		sleep(1);	
		if (msgrcv(msg_id, s_buf, sizeof(s_buf->mtext), id, IPC_NOWAIT) > 0)
			printf("rcv:%s\n", s_buf->mtext);

		printf("choice:[lLCBQF]");
		fflush(stdout);
		fgets(buf, sizeof(buf), stdin);
		c =  buf[0];
		
		switch (c) {
			case 'l':
				login();
				n = strlen(s_buf->mtext) + 1;
				msgsnd(msg_id, s_buf, n, 0);
				break;
			case 'L':
				snprintf(s_buf->mtext, BUF_SIZE, "L:%d:", id);
				n = strlen(s_buf->mtext) + 1;
				msgsnd(msg_id, s_buf, n , 0);
				msgrcv(msg_id, s_buf, sizeof(s_buf->mtext), id, 0);
				printf("List:%s\n", s_buf->mtext);
				break;
			case 'C':
				chat(msg_id);
				break;
			case 'H':
				menu();
				continue;
				break;
			default:
				break;
		}
	}

	return 0;

err:
	return -1;
}
