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

#define   errExit(str)	\
	do { perror(str); exit(-1); }while(0)

#define   SRV_PORT	8888
#define   BUF_SIZE	4096

int main()
{
	int sock_fd, iret;
	socklen_t srv_addr_len, cli_addr_len;
	//1 socket
	sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	
	if (sock_fd  == -1) 
		errExit("socket");
	
	//1.5 端口复用
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
	char  buf[BUF_SIZE] = {0};

	while (1) {
		//sleep(1);
		conn_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_addr_len);
		if (conn_fd == -1) {
			perror("accept");
			continue;
		}
		while ((n = read(conn_fd, buf, BUF_SIZE - 1)) < 0 && errno == EINTR);
		if (n < 0) {
			perror("read");
			break;
		} else if (n == 0) {
			printf("connect closed by peer\n");
			break;
		} else {
			//"\n\0" -- "\0"
			buf[n - 2] = '\0';
			printf("receive from %s:%hu# %s\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buf);
			//buf --> upper 小写字母转大写，回写给对方
			int i;
			for (i = 0; i < n; ++i) {
				buf[i] = toupper(buf[i]);
			}
			write(conn_fd, buf, n - 1);
		}	
		close(conn_fd);	
	}
	
	close(conn_fd);
	iret = shutdown(sock_fd, SHUT_RDWR);
	if (iret == -1) 
		errExit("shutdown");

	return 0;
}
