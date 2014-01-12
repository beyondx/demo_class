#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "utils.h"

static int
listen_socket(int listen_port)
{
	struct sockaddr_in a;
	int s;
	int yes;

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return -1;
	}
	yes = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
				&yes, sizeof(yes)) == -1) {
		perror("setsockopt");
		close(s);
		return -1;
	}
	memset(&a, 0, sizeof(a));
	a.sin_port = htons(listen_port);
	a.sin_family = AF_INET;
	if (bind(s, (struct sockaddr *) &a, sizeof(a)) == -1) {
		perror("bind");
		close(s);
		return -1;
	}
	printf("accepting connections on port %d\n", listen_port);
	listen(s, 10);
	return s;
}

static int handle_client(int conn_fd)
{
	//处理客户端的请求
	char buf[BUF_SIZE] = {0};
	char msg[BUF_SIZE] = {0};
	
	int n;

	n = recv(conn_fd, buf, BUF_SIZE - 1, 0);
	
	if (n < 0) {
		perror("recv");
	} else if (n == 0) {
		printf("connect close by peer\n");
		close(conn_fd);
		//epoll_ctl(epollfd, EPOLL_CTL_DEL, conn_fd, &ev); 
	} else {
		printf("recv:%s\n", buf);
	}

	snprintf(msg, BUF_SIZE -1, "i am server, your conn_id is %d.", conn_fd);
	
	n = send(conn_fd, msg, strlen(msg) + 1, 0);
	printf("send: %s to id %d\n", msg, conn_fd);

	return 0;

err:
	//断开连接
	close(conn_fd);
	return -1;
}



int main()
{
	int sock_fd, n;
	//socket,bind,listen
	CHK2(sock_fd, listen_socket(SRV_PORT));
	
	struct epoll_event ev, events[MAX_EVENTS];
	int conn_sock, nfds, epollfd;
	struct sockaddr_in local;
	socklen_t addr_len = sizeof(local);
	
	//epoll create
	CHK2(epollfd, epoll_create(10));

	ev.events = EPOLLIN;
	ev.data.fd = sock_fd;
	//init
	CHK(epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_fd, &ev));

	for ( ; ;) {
		 nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
               if (nfds == -1) {
                   perror("epoll_pwait");
                   exit(EXIT_FAILURE);
               }

               for (n = 0; n < nfds; ++n) {
                   if (events[n].data.fd == sock_fd) {
                       conn_sock = accept(sock_fd,
                                       (struct sockaddr *) &local, &addr_len);
                       if (conn_sock == -1) {
                           perror("accept");
                           exit(EXIT_FAILURE);
                       }
		//debug
		printf("request from %s: %hu\n", inet_ntoa(local.sin_addr), 
					ntohs(local.sin_port));
			//非阻塞
		       setnonblocking(conn_sock);
		       ev.events = EPOLLIN | EPOLLET;
		       ev.data.fd = conn_sock;
		       if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
					       &ev) == -1) {
			       perror("epoll_ctl: conn_sock");
			       exit(EXIT_FAILURE);
		       }
		   } else {
			   handle_client(events[n].data.fd);
		   }
	      }		
	}

	return 0;
}

