#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>

#define  SRV_PORT 6666
#define  BUF_SIZE 4096

// ./client  ip [port]

int main(int argc, char *argv[])
{
	struct sockaddr_in  srv_addr, cli_addr;
	int sock_fd, n;
	socklen_t addr_len;
	char buf[BUF_SIZE] = {0};

	assert(argc >= 2);

	//1. socket  SOCK_DGRAM
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (sock_fd < 0) {
		perror("socket");
		exit(-1);
	}

	//2. setsockopt  SO_BROADCAST
	int yes = 1;
	n = setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));
	if (n < 0) {
		perror("setsockopt");
		exit(-1);
	}
	
	bzero(&srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(SRV_PORT);
	inet_aton(argv[1], &srv_addr.sin_addr); // 0.0.0.0
	

	addr_len = sizeof(srv_addr);

	int i = 5;
	while (--i) {
		sleep(1);
		time_t t = time(NULL);
		char msg[128] = {0};
		n = snprintf(msg, sizeof(msg) - 1, "%s", ctime(&t));
		msg[n - 1] = '\0';
		n = sendto(sock_fd, msg, strlen(msg) + 1, 0, (const struct sockaddr *)&srv_addr, addr_len);
		if (n < 0) {
			perror("sendto");
			break;
		}
		n = recvfrom(sock_fd, (void *)buf, BUF_SIZE - 1, 0, (struct sockaddr *)&srv_addr, &addr_len);
		
		if (n <= 0) {
			perror("recvfrom");
			break;
		} else {
			buf[n - 1] = '\0';
			printf("client: [%s:%hu]:%s\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buf);
		}
	}

	return 0;
}
