#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <errno.h>

#define  MQ_NAME	"/uplook"
#define  BUFF_SIZE     9000
#define   errExit(str) \
	do {perror(str); exit(-1); }while(0)

int main(int argc, char *argv[])
{
	mqd_t mqd = mq_open(MQ_NAME, O_RDWR | O_CREAT, 0666, NULL);
	
	if (mqd == (mqd_t)-1) {
		errExit("mq_open");
	}

#if 0
	char s[][8] =  {
			"1hello",
			"2world",
			"3linux",
			"4test",
			"5uplook",
		};
	
	int i;
	for (i = 0; i < sizeof s/ sizeof(s[0]) - 1; ++i) {
		mq_send(mqd, s[i], sizeof(s[i]), 4);
	}
	mq_send(mqd, s[i], sizeof(s[i]), 1);
#endif
	char rbuf[BUFF_SIZE] = {0};
	unsigned prio = 0;
	int n = 0;
	while (1) {
		if  ((n = mq_receive(mqd, rbuf, BUFF_SIZE, &prio)) > 0) {
			printf("reveive_num = %d\n", n);
			printf("buf:%s, proj:%u\n", rbuf, prio);
		} else if (n == 0) {
			printf("receive end\n");
			break;
		} else {
			errExit("mq_receive");
		}
	}


	printf("mq_recv over\n");

	mq_close(mqd);
	mq_unlink(MQ_NAME);	

	return 0;
}
