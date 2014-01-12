#pragma once
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define   SRV_PORT	6666
#define   MAX_EVENTS    1000
#define   BUF_SIZE	4096

#define   CHK(expr)	\
	do { if ((expr) < 0) {\
		int old_errno = errno;\
		perror(#expr); exit(old_errno); }\
	 }while(0)

#define   CHK2(ret, expr)	\
	do { if ((ret = (expr)) < 0) {\
		int old_errno = errno;\
		perror(#expr); exit(old_errno); }\
	 }while(0)

inline static int setnonblocking(int conn_sock)
{
        CHK(fcntl(conn_sock, F_SETFL, fcntl(conn_sock, F_GETFL, 0) | O_NONBLOCK));
	return 0;
}

