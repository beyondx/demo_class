#include <stdio.h>
#include <stdlib.h>
#include <ctypes.h>
#include "term.h"

/*上位机
 * #功能需求#
 * 1. 请求响应:200ms间隔主动与下位机联系,获取数据
 * 2. 链路检测:500ms没收到确认, 重发六次, 收动则清零, 否在报警
 * 3. 控制单元: 
 * 	[获取]
 * 	1) 获取下位机 ip地址
 * 	2) 获取下位机 主机名
 * 	3) 获取下位机 时间
 *	[控制]
 *	1) 关机|重启
 *	2) [下载|上传]文件
 *	3) ...
 *
 * #协议#:
 * 起始: 0xb10xb10xb1 [3B]
 * 控制: 控制指令[1B] + 序列号[1B]
 * 	[请求]: 大写字母; 
 * 	[返回]: 小写字母;
 * 数据: 
 * 结束: 0xb20xb20xb2 [3B]
 *
 * ------
 *  ##控制宏##
 *  #define  IP_REQ		'A'
 *  #define  IP_ACK		'a'
 *  #define  HOSTNAME_REQ 	'B'
 *  #define  HOSTNAME_ACK 	'b'
 *  #define  TIME_REQ		'C'
 *  #define  TIME_ACK		'c'
 *  #define  DOWNLOAD_REQ	'D'
 *  #define  DOWNLOAD_ACK	'd'
 *  #define  SHUTDOWN_REQ	'E'
 *  #define  SHUTDOWN_ACK	'e'
 *
 *  #define  TIME_STEP 	200
 *  #define  TIME_OVER	500
 * */

void menu(void)
{
	printf("-------------\n");
	printf("A. 获取IP地址\n");
	printf("B. 获取主机名\n");
	printf("C. 获取时间\n");
	printf("\n");
	printf("D. 下载配置位置\n");
	printf("E. 上传配置位置\n");
	printf("\n");
	printf("F. 关机\n");
	printf("G. 重启\n");
	printf("H. 帮助");
	printf("Q. 退出");
	printf("-------------\n");
}

int main()
{
	char buf[MAX] = {0};
	struct term_t *term = (struct term_t *)malloc(sizeof(struct term_t));
	
	if (term == NULL) {
		ALERT_PRINT("out of memory\n");
	}
	
	term_init(term);

	if (term->open_dev(term, "/dev/ttyUSB0") < 0) {
		ERR_PRINT("open_dev failed\n");
		goto err_open_dev;
	}

	if (term->set_speed(term, 115200) < 0) {
		ERR_PRINT("set_speed failed\n");
		goto err;
	}

	if (term->set_attr(term, 8, 1, 'N') < 0) {
		ERR_PRINT("set_attr failed\n");
		goto err;
	}

	//刷新输入缓冲区
	printf("\n");
	menu();
	char c;

	try:
	while (1) {
		printf("Command (9 for help)\n");
		printf("input your chioce[0-9]:");
		fflush(stdout);
		while (c = getchar()) {
			if (isspace(c) == 0) {
				break;
			}
		}

		switch (c) {
			case '0':
			case 'A':
				//IP
			case '1':
			case 'B':
				//hostname
			case '2':
			case 'C':
				//time
			case '3':
			case 'D':
				//download config
			case '4':
			case 'E':
				//upload config
			case '5':
			case 'F':
				//shutdown 
			case '6':
			case 'G':
				//reboot
			case '0':
			case 'H':
				//quit
				printf("quit\n");
				goto out;
			case '8':
				//TODO
			case '9'://help
			case 'H':
				menu();
				break;
			deafult:
				fprintf(stderr,"out of range %c\n", c);
				goto try;
		}
	}

out:
	term_detroy(term);
	free(term);

	return 0;

err:
err_open_dev:
	return -1;
}


