#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <errno.h>

#define  MQ_NAME	"/uplook"

#define   errExit(str) \
	do {perror(str); exit(-1); }while(0)


int mq_status(mqd_t mqd)
{
	struct mq_attr attr = {0};
	
	if (mq_getattr(mqd, &attr) < 0) {
		perror("mq_getattr");
	}
	
	printf("----status---\n");
	printf("mq_flags = %ld\n", attr.mq_flags);
	printf("mq_maxmsg = %ld\n", attr.mq_maxmsg);
	printf("mq_msgsize = %ld\n", attr.mq_msgsize);
	printf("mq_curmsgs = %ld\n", attr.mq_curmsgs);
	printf("----status end----\n");

	return 0;
}

int main(int argc, char *argv[])
{
	mqd_t mqd = mq_open(MQ_NAME, O_RDWR | O_CREAT, 0666, NULL);
	
	if (mqd == (mqd_t)-1) {
		errExit("mq_open");
	}

	mq_status(mqd);
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
	mq_send(mqd, s[i], sizeof(s[i]), 5);

	printf("mq_send over\n");
	mq_status(mqd);
	

	return 0;
}
