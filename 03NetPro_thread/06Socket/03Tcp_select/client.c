#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>

#define   errExit(str)	\
	do { perror(str); exit(-1); }while(0)

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "缺少参数\n");
		printf("Usage: %s ip port\n", argv[0]);
		exit(-1);
	}
	
	char *srv_ip = argv[1];
	char *srv_port = argv[2];
	char *msg = argv[3];
	int iret;
	int  sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		errExit("socket");
	}
	printf("socket ok, sock_fd:%d\n", sock_fd);

	struct  sockaddr_in srv_addr;
	socklen_t  srv_addr_len = sizeof(srv_addr);

	bzero(&srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	inet_aton(srv_ip, &srv_addr.sin_addr);
	srv_addr.sin_port = htons(atoi(srv_port));
	
	int conn_fd = connect(sock_fd, (const struct sockaddr *)&srv_addr, srv_addr_len);
	if (conn_fd == -1) {
		errExit("connect");
	}
	printf("connect is ok, conn_fd = %d\n", conn_fd);

	char buf[1024] = {0};
	int n;
	
	fd_set rd_set;
	int nfds;
	while (1) {
		/*select 需要重复初始化，故放在循环内部*/
		FD_ZERO(&rd_set);
		FD_SET(STDIN_FILENO, &rd_set);
		FD_SET(sock_fd, &rd_set);
		nfds = sock_fd; //目前最大的文件描述符
		struct timeval tv = {1, 0};
		
		n = select(nfds + 1, &rd_set, NULL, NULL, &tv);
		if (n < 0) {
			perror("select");
			break;
		} else if (n == 0) {
			//printf("time out\n");
			continue;
		} 
		
		if (FD_ISSET(STDIN_FILENO, &rd_set)) {
			// 标准输入有数据
			if (fgets(buf, sizeof(buf) - 1, stdin) != NULL) {
				n = strlen(buf);
				buf[n - 1] = '\0';
				write(sock_fd, buf, n - 1);
				if (strcmp(buf, "exit") == 0) {
					close(sock_fd);
					break;
				}
			}
		}
		if (FD_ISSET(sock_fd, &rd_set)) {
			//socket 有数据
			n = read(sock_fd, buf, sizeof(buf) - 1);
			if (n == 0) {
				printf("connect close by peer\n");
				close(sock_fd);
				break;
			}
			printf("server:%s(%d)\n", buf, n);
		}
	}



#if 0
	while (fgets(buf, sizeof(buf) - 1, stdin) != NULL) {
		n = strlen(buf);
		buf[n - 1] = '\0';
		write(sock_fd, buf, n - 1);
		//printf("write: %s\n", buf);
		bzero(buf, sizeof(buf));
		int n = read(sock_fd, buf, sizeof(buf));
		if (n < 0) {
			perror("read");
			break;
		} else if (n == 0) {
			fprintf(stderr,"connection close by peer\n");
			break;
		} else  {
			//buf[n - 1] = '\0';
			printf("read: %s(%d)\n", buf, n);
		}
	}
#endif
	close(sock_fd);

	return 0;
}
