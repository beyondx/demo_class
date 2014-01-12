#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>

#define  SRV_PORT 6666
#define  BUF_SIZE 4096

int main()
{
	struct sockaddr_in  srv_addr, cli_addr;
	int sock_fd, n;
	socklen_t addr_len;
	char buf[BUF_SIZE] = {0};
	//1. socket  SOCK_DGRAM
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (sock_fd < 0) {
		perror("socket");
		exit(-1);
	}
	
	bzero(&srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(SRV_PORT);
	srv_addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
	//2. bind
	n = bind(sock_fd, (const struct sockaddr *)&srv_addr, sizeof(srv_addr));
	if (n < 0) {
		perror("bind");
		exit(-1);
	}
	
	printf("bind ok! listen: %hd\n", SRV_PORT);

	addr_len = sizeof(cli_addr);

	while (1) {
		//3. block util receive from other clients.
		n = recvfrom(sock_fd, (void *)buf, BUF_SIZE - 1, 0,(struct sockaddr *)&cli_addr, &addr_len);
		
		if (n <= 0) {
			perror("recvfrom");
			continue;
		} else {
			buf[n - 1] = '\0';
			printf("server: [%s:%hu]:%s\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buf);
		}
		time_t t = time(NULL);
		char msg[128] = {0};
		n = snprintf(msg, sizeof(msg) - 1, "%s", ctime(&t));
		msg[n - 1] = '\0';
		n = sendto(sock_fd, msg, strlen(msg) + 1, 0, (const struct sockaddr *)&cli_addr, addr_len);
		if (n < 0) {
			perror("sendto");
		}
	}

	return 0;
}
