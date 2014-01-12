#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "chat.h"
#include <unistd.h>

#define  MY_TYPE  1

LIST_HEAD(head);
struct list_head *phead;
int msg_id = 0;
struct msg_buf *s_buf = NULL;
static volatile int srv_shutdown = 0;
static volatile int srv_stop = 0;

int init_msq(void)
{
	key_t key = ftok(MSQ_NAME, 'a');

	if (key < 0) {
		perror("ftok");
		goto err;
	}

	int msg_id = msgget(0x12345,  IPC_CREAT|0600);

	if (msg_id < 0) {
		perror("msgget");
		goto err;
	}

	return msg_id;
err:
	return -1;
}


void send_msg(int cid, int msgid, struct msg_buf *buf, int buf_len)
{
	buf->mtype = cid;
	msgsnd(msgid, buf, buf_len + 1, 0);
}

static int m_user_online_broadcast()
{
	/*轮询链表，向所有在线用户发送用户登陆消息*/
	struct list_head *cur = NULL;
	struct list_head *last = head.prev;
	struct user_info *plast = container_of(last, struct user_info, list);
	struct msg_buf *buf = (struct msg_buf *)malloc(sizeof(*buf));
	assert(buf != NULL);

	snprintf(buf->mtext, BUF_SIZE, "B:00:%s is online, id(%d)\n", plast->name, plast->id);
	list_for_each(cur, &head) {
		struct user_info *pcur = container_of(cur, struct user_info, list);
		send_msg(pcur->id, msg_id, buf, strlen(buf->mtext));
#ifdef DEBUG
	printf("server broadcast to %d: %s\n", pcur->id, buf->mtext);
#endif
	}
	free(buf);
}

static int m_list_user(int id)
{
	struct msg_buf *buf = (struct msg_buf *)malloc(sizeof(*buf));
	char tmp[512]= {0};
	buf->mtype = id;
	struct list_head *cur = NULL;
	snprintf(buf->mtext, BUF_SIZE, "L:00:");

	list_for_each(cur, &head) {
		struct user_info *pu = container_of(cur, struct user_info, list);
		#ifdef DEBUG
		printf("id:%d\n", pu->id);
		printf("name:%s\n", pu->name);
		#endif
		sprintf(tmp, "#%d#%s$", pu->id, pu->name);
		strncat(buf->mtext, tmp, BUF_SIZE);
	}
#ifdef DEBUG
	printf("buf->mtext:%s\n", buf->mtext);
#endif
	send_msg(id, msg_id, buf, strlen(buf->mtext));
	free(buf);
}

static int m_login(char *msg)
{
	/*客户端申请登陆*/
	#ifdef DEBUG
	printf("%s:%s\n", __FUNCTION__, msg);
	#endif

	struct user_info *user = (struct user_info *)malloc(sizeof(*user));
	assert(user != NULL);
	msg[2] = '\0';
	char *pname = msg + 3;
	user->id = atoi(msg);
	strncpy(user->name, pname, MAX_NAME);
	list_add_tail(&head, &(user->list));
	/*广播通知*/
	m_user_online_broadcast();
	return 0;
}

static int m_chat(char *msg)
{
	#ifdef DEBUG
		printf("%s:%s\n", __FUNCTION__, msg);
	#endif
	msg[2] = '\0';
	int dst_id = atoi(msg);
	strncpy(s_buf->mtext, msg + 3, BUF_SIZE);
	send_msg(dst_id, msg_id, s_buf, strlen(s_buf->mtext));	
}

static int msg_handler(const char *msg)
{
	int id;
	assert(msg != NULL);
	/* 消息处理 */	
	switch (msg[0]) {
		case 'l':
			/*login*/
			m_login(msg + 5);
			break;
		case 'L':
			id = strtol(msg + 2, NULL, 10);
			#ifdef DEBUG
				printf("cid = %d\n",id);
			#endif
			m_list_user(id);
			break;
		case 'Q':
			/*quit*/
			//m_quit(msg + 3);
			break;
		case 'C':
			/*chat*/
			m_chat(msg + 3);
			break;
		case 'B':
			/*broadcast*/
			//m_send_msg();
			break;
		case 'F':
			/*file-transfer*/
			break;
		default:
			fprintf(stderr, "unkown msg type:%c\n", msg[0]);
			goto err;
			break;
	}

	return 0;
err:
	return -1;
}

int server_destroy()
{
	if ((n = msgctl(msg_id, IPC_RMID, NULL) < 0)) {
		perror("msgctl");
		goto err;
	}
	return 0;

err:
	return -1;
}

void sig_handler(int signo)
{
	switch (signo) {
		case SIGINT:
		case SIGQUIT:
			srv_shutdown = 1;
			break;
		default:
			break;
	}
}

int server_init()
{
	msg_id =  init_msq();

	if (msg_id < 0) {
		fprintf(stderr, "init_msq failed\n");
		goto err;
	}
	
	phead = (char *)malloc(sizeof(*phead));
	assert(phead != NULL);
	
	init_list_head(phead);
	signal(SIGINT, sig_handler);
	signal(SIGQUIT, sig_handler);
	signal(SIGPIPE, SIG_IGN);

err:
	return -1;
}

void show_buf(const struct msg_buf *msg)
{
	printf("-----------msg_buf---------------\n");
	printf("cmd: dst_id: stime: key: msg_size: msg\n");
	printf("%c:%hu:%lu:%x:%hu:%s\n", msg->mbuf.msg.cmd, msg->mbuf.msg.dst_id, msg->mbuf.msg.stime, msg->mbuf.msg.key, msg->mbuf.msg.msg_size, msg->mbuf.msg.buf);
}

int main()
{
	server_init();
	s_buf = (struct msg_buf *)malloc(sizeof(struct msg_buf));
	assert(s_buf != NULL);

	int n;
	printf("server start(pid=%d)\n", getpid());
	

	while (!srv_shutdown) {

		n = msgrcv(msg_id, s_buf, BUF_SIZE, 0, 0);
		if (n < 0) {
			perror("msgrcv");
			if (errno == EINTR) {
				continue;
			}
			break;
		}
	#ifdef DEBUG
		printf("type:%d,n:%d:text:%s\n", s_buf->mtype, n, 
				s_buf->mtext);
	#endif
		/*解析消息*/
		if (msg_handler(s_buf->mtext) < 0) {
			fprintf(stderr,"msg_handler error:%s\n", s_buf->mtext);
		}
	}

	printf("server shutdown \n");	

	free(s_buf);
	return 0;

err:
	server_destroy();
	return -1;
}
