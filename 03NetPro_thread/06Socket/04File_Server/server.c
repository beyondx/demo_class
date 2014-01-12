#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define   errExit(str)	\
	do { perror(str); exit(-1); }while(0)

#define   SRV_PORT	8888
#define   BUF_SIZE	4096
#define  MAX_CLI_NUM 	2000

char buf[BUF_SIZE] = {0};
static int ol_con = 0;
int cli_fds[MAX_CLI_NUM];
fd_set rd_set;
size_t n_handlers = 0;


char *get_filename(const char *str)
{
	char *str_start = strstr(str, "GET /");
	
	if (str_start != NULL) {
		char *filename = str_start + strlen("GET /");
		char *str_end = strchr(filename, ' ');
		if (str_end != NULL) {
			*str_end = '\0';
		} else {
			fprintf(stderr, "Invalid request!, 'space' not found\n");
			return NULL;
		} 
		printf("Filename:'%s'\n", filename);	
		return filename;
	} 
		return NULL;

}
//read, write
static int file_send(int conn_fd, const char *file)
{
	int fd = open(file, O_RDONLY);
	
	if (fd == -1) {
		perror("open");
		return -1;
	}
	memset(buf, 0, sizeof(buf));
	int n = 0;
	while ((n = read(fd, buf, sizeof(buf))) != 0) {
		write(conn_fd, buf, n);
	}

	close(fd);
	return 0;
}
//mmap
static int file_send2(int conn_fd, const char *file)
{
	int iFd = open(file, O_RDONLY);
	if (iFd < 0) {
		printf("open %s failed: errno:%d, errMsg:%s\n", file, errno, strerror(errno));
		return -1;
	}
	size_t iFileSize = lseek(iFd,  0, SEEK_END);
	printf("iFileSize=%lu\n", iFileSize);

	char *pFile = mmap(NULL, iFileSize, PROT_READ, MAP_PRIVATE, iFd, 0);
	if (pFile == MAP_FAILED) {
		fprintf(stderr, "mmap failed:%s\n", strerror(errno));
		return -1;
	}
	int iret = 0;
	iret = write(conn_fd, pFile, iFileSize);	
	if (iret < 0) {
		perror("write");
		return -1;
	}
	munmap(pFile, iFileSize);

	return 0;
}

int server_close(int conn_fd)
{
	int n;
	for (n = 0; n <= ol_con; ++n) {
		if (cli_fds[n] == conn_fd) {
			cli_fds[n] = -1;
			FD_CLR(conn_fd, &rd_set);
		}
	}
	printf("%s,ol_con:%d\n", __FUNCTION__, --ol_con);
	close(conn_fd);
	return 0;
}

static int client_handler(int conn_fd)
{
	printf("%s\n", __FUNCTION__);
	int n;
	// loop 
		while ((n = read(conn_fd, buf, BUF_SIZE - 1)) < 0 && (errno == EINTR || errno == EAGAIN));
		if (n < 0) {
			perror("read error");
			goto err;
		} else if (n == 0) {
			printf("connect closed by peer\n");
			//TODO　 FIXME, BUG:客户端退出后需要更新cli_fds
			server_close(conn_fd);
		} else {
			//GET / HTTP/1.1
			
			printf("handle_nums = %lu\n", ++n_handlers);
			char file[255] = {0};
			char *p = get_filename(buf);
			if (p == NULL) {
				fprintf(stderr, "file not found\n");
				goto err;
			}
			printf("file_name=%s\n", strncpy(file, p, 255));
			if (file_send2(conn_fd, file) < 0) {
				server_close(conn_fd);
			}
		}
		
	return 0;
err:
	server_close(conn_fd);
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
	iret = listen(sock_fd, 100);
	if (iret == -1)
		errExit("listen");

#ifdef  _DEBUG
	printf("listen is ok\n");
#endif
	int  conn_fd, n;
	pid_t child_pid;
	/*数组用来保存每一个连接的文件描述符*/
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
		struct timeval timeout = {1, 0};

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
		char add_info[1024] = {0};
		printf("pid = %d, receive from %s:%hu\n", getpid(), inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
		//fflush(stdout);
		#if 1
		printf("ol_con = %d, line:%d\n", ol_con, __LINE__);
		if (ol_con  == MAX_CLI_NUM) {
			snprintf(buf, BUF_SIZE - 1,"Warn, connect nums has over the limits: %d, max:(%d)\n", ol_con, MAX_CLI_NUM);
			write(conn_fd, buf, strlen(buf) + 1);
			close(conn_fd);
		} else {
			for (i = 0; i < MAX_CLI_NUM; ++i) {
				if (cli_fds[i] < 0) {
					cli_fds[i] = conn_fd;
					break;
				}
			}	
			++ol_con;
		}
		#endif
		}

		for (i = 0; i < ol_con; ++i) {
			if (FD_ISSET(cli_fds[i], &rd_set)) {
				printf("request: %d\n", cli_fds[i]);
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
