#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define   errExit(str)	\
	do { perror(str); exit(-1); }while(0)

#define   SRV_PORT	8888
#define   BUF_SIZE	4096

char buf[BUF_SIZE] = {0};
static int ol_con = 0;
int cli_fds[MAX_CLI_NUM];

static int client_handler(int conn_fd)
{
	int n;
	// loop 
		while ((n = read(conn_fd, buf, BUF_SIZE - 1)) < 0 && errno == EINTR);
		if (n < 0) {
			perror("read");
			goto err;
		} else if (n == 0) {
			printf("connect closed by peer\n");
			--ol_con;
			//TODO　 FIXME, BUG:客户端退出后需要更新cli_fds
			close(conn_fd);
		} else {
			//"\n\0" -- "\0"
			buf[n] = '\0';
			printf("%s(%d)\n", buf, n);
			//buf --> upper 小写字母转大写，回写给对方
			int i;
			for (i = 0; i < n; ++i) {
				buf[i] = toupper(buf[i]);
			}
			write(conn_fd, buf, n);
		}
		
	return 0;
err:
	close(conn_fd);
	return -1;
}


int main()
{
	int sock_fd, iret;
	socklen_t srv_addr_len, cli_addr_len;

	//1 socket
	sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	
	if (sock_fd  == -1) 
		errExit("socket");
	
	//1.5 地址复用,避免2MSL时间未超时，而导致服务进程无法再次启动
	// 但是不能同时运行两个不同的进程，去抢占同一个IP的同一个端口
	int optval = 1;
	iret = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	
	if (iret == -1) {
		errExit("setsockopt");
	}
	
#ifdef _DEBUG
	printf("socket ok, sockfd: %d\n", sock_fd);
#endif
	struct sockaddr_in  srv_addr, cli_addr;
	bzero(&srv_addr, sizeof(srv_addr));

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(SRV_PORT);
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0--->0 
	
	//2 bind
	srv_addr_len = sizeof(srv_addr);
	iret = bind(sock_fd, (const struct sockaddr *)&srv_addr, srv_addr_len);

	if (iret == -1) 
		errExit("bind");
	
#ifdef  _DEBUG
	printf("bind is ok\n");
#endif	
	
	//3. listen
	iret = listen(sock_fd, 10);
	if (iret == -1)
		errExit("listen");

#ifdef  _DEBUG
	printf("listen is ok\n");
#endif
	int  conn_fd, n;
	pid_t child_pid;
	fd_set rd_set;
	/*数组用来保存每一个连接的文件描述符*/
#define  MAX_CLI_NUM 	2
	int nfds = sock_fd;
	// ol_con : 客户端有效连接数
	int i = 0;
	for (i = 0; i < MAX_CLI_NUM; ++i) {
		cli_fds[i] = -1;
	}	

	while (1) {
		FD_ZERO(&rd_set);
		FD_SET(sock_fd, &rd_set);
		//找出最大的文件描述符，将现有的sock连接描述符添加到监控集中
		for (i = 0; i < MAX_CLI_NUM; ++i) {
			if (cli_fds[i] > 0) {
				FD_SET(cli_fds[i], &rd_set);
			}
			if (cli_fds[i] > nfds) {
				nfds = cli_fds[i];
			}
		}
		//超出服务数限制
		struct timeval timeout = {3, 0};

		iret = select(nfds + 1, &rd_set, NULL, NULL, &timeout);	
	
		if (iret < 0) {
			perror("select");
			break;
		} else if (iret  == 0) {
			printf(".");
			fflush(stdout);
			continue;
		}

		if (FD_ISSET(sock_fd, &rd_set)) {
			cli_addr_len = sizeof(cli_addr);
			conn_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_addr_len);
			if (conn_fd == -1) {
				perror("accept");
				break;
			}
		printf("pid = %d, receive from %s:%hu#", getpid(), inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
		fflush(stdout);
		#if 1
		if (ol_con  == MAX_CLI_NUM) {
			snprintf(buf, BUF_SIZE - 1,"Warn, connect nums has over the limits: %d, max:(%d)\n", ol_con, MAX_CLI_NUM);
			write(conn_fd, buf, strlen(buf) + 1);
			close(conn_fd);
		} else {
			cli_fds[ol_con++] = conn_fd;	
		}
		#endif
		}

		for (i = 0; i < ol_con; ++i) {
			if (FD_ISSET(cli_fds[i], &rd_set)) {
				client_handler(cli_fds[i]);
			}
		}
	}

	close(conn_fd);
	iret = shutdown(sock_fd, SHUT_RDWR);
	if (iret == -1) 
		errExit("shutdown");

	return 0;
}
