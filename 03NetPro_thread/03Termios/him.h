#pragma once
#include <stdio.h>
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
 *  #define  UPLOAD_REQ		'E'
 *  #define  DOWNLOAD_ACK	'd'
 *  #define  UPLOAD_ACK		'e'
 *  #define  SHUTDOWN_REQ	'F'
 *  #define  SHUTDOWN_ACK	'f'
 *  #define  REBOOT_REQ		'G'
 *  #define  REBOOT_ACK		'g'		
 *
 *  #define  TIME_STEP 	200
 *  #define  TIME_OVER	500
 * */

